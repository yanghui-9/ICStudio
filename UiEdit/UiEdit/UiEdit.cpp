#include "UiEdit.h"
#include "MainWindow.h"
#include "Itemsfactory.h"

#include "extend_func_head.h"

IUiModel *CreateUiM()
{
    return new UiEdit();
}

UiEdit::UiEdit()
{
}

UiEdit::~UiEdit()
{
    if(m_mainW)
    {
        delete m_mainW;
    }
}

int UiEdit::Init(std::shared_ptr<interface_comm> &comm)
{
    m_comm = comm;
    m_mainW = new MainWindow(comm);
    m_mainW->initUI();
    m_mainW->show();

    //初始化外调参数
    InitParms();

    return 0;
}

int UiEdit::ExecFuncFromID(int funID, void **_a)
{
    int ret = 0;
    switch (funID)
    {
    case 0: {bool ret = SwitchScene(Z_TRANSFORM(const std::string,1));Z_TRANSFORM_R(bool,ret);}  break;
    default: ret = -1;
    }
    return ret;
}

const char *UiEdit::CheckParms(int funID)
{
    if(static_cast<uint64_t>( funID ) >= m_parms.size())
    {
        return nullptr;
    }
    return m_parms[static_cast<uint32_t>( funID )].data();
}

int UiEdit::GetFuncID()
{
    return 0 + 0x16FF;
}

std::string UiEdit::GetFuncName()
{
    return  "UI";
}

bool UiEdit::SwitchScene(const std::string &name)
{
    return m_mainW->SwitchScene(name);
}
