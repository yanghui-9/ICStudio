#include <QCoreApplication>
#include <QProcess>
#include <QFile>
#include <QJsonParseError>
#include <QJsonObject>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //读配置文件获取待加载模块名.
    // 打开json文件.
    QFile jsonFIle("system/systemConfig.json");
    if( jsonFIle.open(QIODevice::ReadWrite))
    {
        QJsonParseError jsonParserError;
        QJsonDocument outDoc = QJsonDocument::fromJson(jsonFIle.readAll(),&jsonParserError);
        // 文档不空 && jsonParserError解析没有错误，解析成功返回true.
        if(!outDoc.isNull() && (jsonParserError.error == QJsonParseError::NoError))
        {
            QJsonObject objRoot =  outDoc.object();
            QJsonObject objSys = objRoot["systemParams"].toObject();
            objSys["model"] = 1;
            objRoot["systemParams"] = objSys;
            jsonFIle.close();
            //json转字节
            if( jsonFIle.open(QIODevice::WriteOnly))
            {
                outDoc.setObject(objRoot);
                jsonFIle.write(outDoc.toJson());
            }
        }
        jsonFIle.close();
    }

    int ret = QProcess::execute("ICStudio.exe");

    return ret;
}
