#include "AppRun.h"
#include "event_def.h"
#include "FileCommunication/CommunicationManagement.h"
#include <QDebug>

unsigned short _cal_crc_(unsigned char *data, unsigned short len)
{
    unsigned short crc_ = 0xffff;
    int i,j;
    for (i=0; i<len; i++)
    {
        crc_ ^= data[i];
        for (j=0; j<8; j++)
        {
            if (crc_ & 1)
            {
                crc_ >>= 1;
                crc_ ^= 0xA001;
            }
            else
            {
                crc_ >>= 1;
            }
        }
    }

    return crc_;
}

AppRun::AppRun(int n_threads):IAppR (n_threads)
{
    //IAPP::Instance = this;
}

AppRun::~AppRun()
{
    m_funcModel.clear();
   //卸载外设.
   m_comm->UnInit();
   //处理线程
   if(m_processThread)
   {
       m_bThreadFlag = false;
       m_processThread->join();
       m_processThread.reset();
   }
}

void AppRun::InitRun()
{
    //1.初始化配置,读配置文件.

    //2.外设初始化.
    m_comm = std::make_shared<CommunicationManagement>();
    if(m_comm)
    {
        m_comm->Init();
    }

    //3.UI创建.
    m_UiSupport = std::shared_ptr<IUiModel>(m_uiSupportManager.InitLoad());
    m_funcModel.push_back(m_UiSupport);

    //4.后台插件创建.
    m_funcManager.InitLoad(m_funcModel);

    //5.建立扩展模块索引对应关系.
    BuildingModelOfIndex();

    //6.建立事件绑定.
    BuildingEventBind();

    //7.初始化ui和后台插件
    foreach (auto func, m_funcModel) {
        FuncModel * model = dynamic_cast<FuncModel *>(func.get());
        if(model)
        {
            model->Init(m_comm);
        }
    }
    if(m_UiSupport)
    {
        m_UiSupport->Init(m_comm);
    }

    //int i = 0;
    //IAPP::InvokeMethod(0,Z_RETURN_ARG(int,i),Z_ARG(int,1),Z_ARG(int32_t,20));
}

int AppRun::GetModelFromName(const std::string &name, std::shared_ptr<ZObject> &model)
{
    for (const auto &iModel : m_funcModel) {
        if(iModel->GetFuncName() == name){
            model = iModel;
            return 0;
        }
    }
    return -1;
}

bool AppRun::Event(HEvent *event)
{
    bool bDeal = true;
    DealRegisteredEvents(event);
    return bDeal;
}

void AppRun::DealRegisteredEvents(HEvent *event)
{
    std::map<int ,std::vector<std::shared_ptr<ZObject>> >::iterator it = m_eventRegisterMap.find(event->_type);
    if(it != m_eventRegisterMap.end())
    {
        for(uint i = 0; i < it->second.size() ; i++)
        {//执行事件回调
            it->second.at(i)->EventCall(event);
        }

        //执行完成回调
        if( (event)->_send )
        {
            event->_send->EventCall(event);
        }
    }
}

void AppRun::BuildingModelOfIndex()
{
    //建立扩展模块索引对应关系
    int index = 0;
    foreach (auto model, m_funcModel) {
       index = model->GetFuncID();
       if(-1 == index)
       {
           continue;
       }
       DecryptionFuncIdFromFuncName(index,model->GetFuncName());
       if(index < 0 || index % 256 != 0)
       {//id必须为256的整数倍
           continue;
       }
       index = (index>>8);
       if( static_cast<uint64_t>(index) >= m_models.size())
       {
           m_models.resize(static_cast<uint64_t>(index)+1);
       }
       m_models[static_cast<uint64_t>(index)] = model;
    }
}

void AppRun::DecryptionFuncIdFromFuncName(int &index, const std::string &name)
{
    if(name.empty())
    {
        index = index - 1;
    }
    else
    {
        ushort crc16 = _cal_crc_(reinterpret_cast<unsigned char *>( const_cast<char *>(name.data()) ),name.size());
        index = index - crc16;
    }
}

void AppRun::DealModelCycleRun()
{
    int cycTime = 0;
    CycRunModelType runModelData;
    foreach (auto model, m_funcModel) {
       FuncModel *func = dynamic_cast<FuncModel *>(model.get());
       if(func)
       {
           cycTime = func->GetRunIntervalTime();
           if(-1 <= cycTime)
           {//需要循环调用模块
               runModelData.model = func;
               runModelData.CycleIntervalTime = cycTime;
               runModelData.RecentSchedulingTime = QDateTime::fromString(QString("1970-01-01-00-00-00"), "yyyy-MM-dd-HH-mm-ss");
               m_cycleRunModels.push_back(runModelData);
           }
       }
    }

    if(0 < m_cycleRunModels.size())
    {//启动线程
        m_processThread = std::make_shared<std::thread>(&AppRun::ModelCycleRun,this);
    }
}

void AppRun::ModelCycleRun()
{
    QDateTime curTime;
    while (m_bThreadFlag) {
        curTime =  QDateTime::currentDateTime();
        for(std::vector<CycRunModelType>::iterator model  = m_cycleRunModels.begin(); model != m_cycleRunModels.end(); ++model)
        {
            if( model->RecentSchedulingTime.msecsTo( curTime ) >=  model->CycleIntervalTime ||
                model->RecentSchedulingTime.msecsTo( curTime ) < 0)
            {
                if(-1 == model->model->Run())
                {//该模块退出循环调用
                    m_cycleRunModels.erase(model);
                    model--;
                    continue;
                }
                else
                {
                    model->RecentSchedulingTime = curTime;
                }
            }
        }
        if(0 == m_cycleRunModels.size())
        {
           m_bThreadFlag = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void AppRun::BuildingEventBind()
{
    int index = 0;
    std::vector<int> eventList;
    foreach (auto model, m_funcModel) {
       eventList.clear();
       index = model->RegisterEvent(eventList);
       if(-1 == index)
       {
           continue;
       }
       foreach (auto eventID, eventList) {
          std::map<int ,std::vector<std::shared_ptr<ZObject>> >::iterator it = m_eventRegisterMap.find(eventID);
          if(it != m_eventRegisterMap.end())
          {
             it->second.push_back(model);
          }
          else
          {
              std::vector<std::shared_ptr<ZObject>> models;
              models.push_back(model);
              m_eventRegisterMap.insert(std::pair<int,std::vector<std::shared_ptr<ZObject>>>(eventID,models));
          }
       }
    }
}
