#ifndef MACROCONFIG
#define MACROCONFIG
#include <QString>


////0-下拉框，1-编辑框
////0-编辑框数字类型，1-编辑框字符串类型
////0-必填参数，1-可选填参数（有默认参数）2-可变参数（个数不定）
////1.操作组态工程参数
const int MACROFUNCTYPE1NUM = 3;
const QString MACROFUNCTYPE1NAME_EN[] =
{

      /*【功能优化】_脚本功能添加命令到下拉框中_yangjindong_20180917_Begin*/
    "getHmiModel@getHmiModel,Obtain the device model#0#",
    "setSystemParameter@setSystemParameter,Set system parameters\n   Parameter 1 path: parameter path to be set\n   Parameter 2 value: set data\n   Parameter 3 valueType: type of data, 0-integer data, 1-float type, 2-string\n   Parameter 4 saveFlag: whether the set value is saved, 0-not saved, 1-save\n   Return value: the result of the setting\n\n\n(This function does not support online emulation)#4#path_0_1_0_0_link-Link1-linkName_link-Link1-linkType_link-Link1-linkDevice_link-Link1-linkDriver_link-Link1-HMIPosition_link-Link1-HMIIP_link-Link1-HMIPort_link-Link1-BaudRate_link-Link1-DataBit_link-Link1-Check_link-Link1-StopBit_link-Link1-ScreenID_link-Link1-PLCID_link-Link1-CommTime_link-Link1-Timeout1_link-Link1-Timeout2_link-Link1-RetryTimes_link-Link1-AddressMode_link-Link1-AddressStep_HMI Sys Setting-Screen Model_userExit_HMI Sys Setting-Splash Screen Index_HMI Sys Setting-Guard Times_HMI Sys Setting-Touch Voice_HMI Sys Setting-Screen Protection Use_HMI Sys Setting-Screen Protection Index_HMI Sys Setting-Load Logo Screen Use_HMI Sys Setting-Load Logo Screen Index_HMI Sys Setting-alarm voice_HMI Sys Setting-alarm blink_HMI Sys Setting-alarm bg color_HMI Sys Setting-Flicker Frequency_HMI Sys Setting-System Alarm-Use System Alarm_HMI Sys Setting-System Alarm-Alarm background_HMI Sys Setting-System Alarm-Alarm text color_HMI Sys Setting-System Alarm-Family_HMI Sys Setting-System Alarm-Font size_HMI Sys Setting-System Alarm-Display mode_HMI Sys Setting-System Alarm-Display position_HMI Sys Setting-cursor hide_HMI Sys Setting-font-font~family_HMI Sys Setting-font-font~pixelSize_HMI Sys Setting-font-font~color_HMI Sys Setting-font-font~letterSpacing_HMI Sys Setting-font-text~position_HMI Sys Setting-font-font~bold_HMI Sys Setting-font-font~underline_HMI Sys Setting-font-font~italic_pictureZoom_HMI Sys Setting-downUpdateRecipe,value_1_0_0_,valueType_0_0_0_0_0_1_2,saveFlag_0_0_0_0_0_1",
    "getSystemParameter@getSystemParameter,Get system parameters\n   Parameter 1 path: parameter path to be set\n   Parameter 2 valueType: type of data, 0-integer data, 1-float type, 2-string\n   Return value: the value obtained#2#path_0_1_0_0_link-Link1-linkName_link-Link1-linkID_link-Link1-linkType_link-Link1-linkDevice_link-Link1-linkDriver_link-Link1-HMIPosition_link-Link1-HMIIP_link-Link1-HMIPort_link-Link1-BaudRate_link-Link1-DataBit_link-Link1-Check_link-Link1-StopBit_link-Link1-ScreenID_link-Link1-PLCID_link-Link1-CommTime_link-Link1-Timeout1_link-Link1-Timeout2_link-Link1-RetryTimes_link-Link1-AddressMode_link-Link1-AddressStep_HMI Sys Setting-Screen Model_userExit_HMI Sys Setting-Splash Screen Index_HMI Sys Setting-Guard Times_HMI Sys Setting-Touch Voice_HMI Sys Setting-Screen Protection Use_HMI Sys Setting-Screen Protection Index_HMI Sys Setting-Load Logo Screen Use_HMI Sys Setting-Load Logo Screen Index_HMI Sys Setting-alarm voice_HMI Sys Setting-alarm blink_HMI Sys Setting-alarm bg color_HMI Sys Setting-Flicker Frequency_HMI Sys Setting-System Alarm-Use System Alarm_HMI Sys Setting-System Alarm-Alarm background_HMI Sys Setting-System Alarm-Alarm text color_HMI Sys Setting-System Alarm-Family_HMI Sys Setting-System Alarm-Font size_HMI Sys Setting-System Alarm-Display mode_HMI Sys Setting-System Alarm-Display position_HMI Sys Setting-cursor hide_HMI Sys Setting-font-font~family_HMI Sys Setting-font-font~pixelSize_HMI Sys Setting-font-font~color_HMI Sys Setting-font-font~letterSpacing_HMI Sys Setting-font-text~position_HMI Sys Setting-font-font~bold_HMI Sys Setting-font-font~underline_HMI Sys Setting-font-font~italic_pictureZoom_HMI Sys Setting-downUpdateRecipe,valueType_0_0_0_0_0_1_2"
};
const QString MACROFUNCTYPE1NAME_CN[] =
{
    QString::fromUtf8("getHmiModel@getHmiModel,获取HMI型号#0#"),
//    QString::fromUtf8("setSystemParameter@setSystemParameter,设置系统参数\n  参数1 path：需要设置的参数路径\n  参数2 value：设置的数据\n  参数3 valueType：数据的类型，0-整型数据，1-浮点型，2-字符串\n  参数4 saveFlag：设置的值是否保存，0-不保存，1-保存\n  返回值：设置的结果#4#path_0_1_0_,value_1_0_0_,valueType_0_0_0_0_0_1_2,saveFlag_0_0_0_0_0_1"),
    QString::fromUtf8("setSystemParameter@setSystemParameter,设置系统参数\n  参数1 path：需要设置的参数路径\n  参数2 value：设置的数据\n  参数3 valueType：数据的类型，0-整型数据，1-浮点型，2-字符串\n  参数4 saveFlag：设置的值是否保存，0-不保存，1-保存\n  返回值：设置的结果\n\n\n(此函数不支持在线模拟)#4#path_0_1_0_0_link-Link1-linkName_link-Link1-linkType_link-Link1-linkDevice_link-Link1-linkDriver_link-Link1-HMIPosition_link-Link1-HMIIP_link-Link1-HMIPort_link-Link1-BaudRate_link-Link1-DataBit_link-Link1-Check_link-Link1-StopBit_link-Link1-ScreenID_link-Link1-PLCID_link-Link1-CommTime_link-Link1-Timeout1_link-Link1-Timeout2_link-Link1-RetryTimes_link-Link1-AddressMode_link-Link1-AddressStep_HMI Sys Setting-Screen Model_userExit_HMI Sys Setting-Splash Screen Index_HMI Sys Setting-Guard Times_HMI Sys Setting-Touch Voice_HMI Sys Setting-Screen Protection Use_HMI Sys Setting-Screen Protection Index_HMI Sys Setting-Load Logo Screen Use_HMI Sys Setting-Load Logo Screen Index_HMI Sys Setting-alarm voice_HMI Sys Setting-alarm blink_HMI Sys Setting-alarm bg color_HMI Sys Setting-Flicker Frequency_HMI Sys Setting-System Alarm-Use System Alarm_HMI Sys Setting-System Alarm-Alarm background_HMI Sys Setting-System Alarm-Alarm text color_HMI Sys Setting-System Alarm-Family_HMI Sys Setting-System Alarm-Font size_HMI Sys Setting-System Alarm-Display mode_HMI Sys Setting-System Alarm-Display position_HMI Sys Setting-cursor hide_HMI Sys Setting-font-font~family_HMI Sys Setting-font-font~pixelSize_HMI Sys Setting-font-font~color_HMI Sys Setting-font-font~letterSpacing_HMI Sys Setting-font-text~position_HMI Sys Setting-font-font~bold_HMI Sys Setting-font-font~underline_HMI Sys Setting-font-font~italic_pictureZoom_HMI Sys Setting-downUpdateRecipe,value_1_0_0_,valueType_0_0_0_0_0_1_2,saveFlag_0_0_0_0_0_1"),
    /*【功能优化】_脚本功能添加命令到下拉框中_yangjindong_20180917_End*/
    QString::fromUtf8("getSystemParameter@getSystemParameter,获取系统参数\n  参数1 path：需要设置的参数路径\n  参数2 valueType：数据的类型，0-整型数据，1-浮点型，2-字符串\n  返回值：获取的值\n#2#path_0_1_0_0_link-Link1-linkName_link-Link1-linkID_link-Link1-linkType_link-Link1-linkDevice_link-Link1-linkDriver_link-Link1-HMIPosition_link-Link1-HMIIP_link-Link1-HMIPort_link-Link1-BaudRate_link-Link1-DataBit_link-Link1-Check_link-Link1-StopBit_link-Link1-ScreenID_link-Link1-PLCID_link-Link1-CommTime_link-Link1-Timeout1_link-Link1-Timeout2_link-Link1-RetryTimes_link-Link1-AddressMode_link-Link1-AddressStep_HMI Sys Setting-Screen Model_userExit_HMI Sys Setting-Splash Screen Index_HMI Sys Setting-Guard Times_HMI Sys Setting-Touch Voice_HMI Sys Setting-Screen Protection Use_HMI Sys Setting-Screen Protection Index_HMI Sys Setting-Load Logo Screen Use_HMI Sys Setting-Load Logo Screen Index_HMI Sys Setting-alarm voice_HMI Sys Setting-alarm blink_HMI Sys Setting-alarm bg color_HMI Sys Setting-Flicker Frequency_HMI Sys Setting-System Alarm-Use System Alarm_HMI Sys Setting-System Alarm-Alarm background_HMI Sys Setting-System Alarm-Alarm text color_HMI Sys Setting-System Alarm-Family_HMI Sys Setting-System Alarm-Font size_HMI Sys Setting-System Alarm-Display mode_HMI Sys Setting-System Alarm-Display position_HMI Sys Setting-cursor hide_HMI Sys Setting-font-font~family_HMI Sys Setting-font-font~pixelSize_HMI Sys Setting-font-font~color_HMI Sys Setting-font-font~letterSpacing_HMI Sys Setting-font-text~position_HMI Sys Setting-font-font~bold_HMI Sys Setting-font-font~underline_HMI Sys Setting-font-font~italic_pictureZoom_HMI Sys Setting-downUpdateRecipe,valueType_0_0_0_0_0_1_2")
};

////2.操作组态工程运行
const int MACROFUNCTYPE2NUM = 18;
const QString MACROFUNCTYPE2NAME_EN[] =
{/*bug3161  脚本中，操作组态工程运行 readdevice函数的介绍中的，第二种形态的参数二应该放在最后_yangjindong_20180921_begin*/
    "setdata@setdata,Set internal and external address value\nFirst form\n   Parameter 1 link: the id or connection name of the connection\n   Parameter 2 reg: register name\n   Parameter 3 index : register index\n   Parameter 4 station : equipment station number\n   Parameter 5 len : write length in words\n    Parameter 6 data: written data\n    Parameter 7 DataType: Data type written\n  Return：null\nSecond form\n  Parameter 1 link: the id or connection name of the connection\n   Parameter 2 reg: register name\nParameter 3 index : register index\n   Parameter 4 station : equipment station number\n   Parameter 5 len : number of data written\n   Parameter 6 array: array written\n    Parameter 7 DataType: Data type written\n  Return：null#7#link_0_1_0_0,reg_0_1_0_0,index_1_0_0_0,station_1_0_0_1,len_1_0_0_1,data_1_0_0_0,DataType_0_0_0_4",
    "getdata@getdata,Get internal and external address value\nFirst form\n   Parameter 1 link: the id or connection name of the connection\n   Parameter 2 reg: register name\n   Parameter 3 index : register index\n   Parameter 4 station : equipment station number\n   Parameter 5 len : read length in words\n    Parameter 6 DataType: read data type\n  Parameter 7 type: type, trigger-trigger class, cycle-loop,In general, the default parameters are fine, and CYCLE is recommended if the loop script is used and the cycle time is short\n  Return：When the script type is a trigger class, the function will have two return values, which are the obtained value and the result of the acquisition, and the result is 1-success, -1-failed.\nSecond form\n  Parameter 1 link: the id or connection name of the connection\n   Parameter 2 reg: register name\nParameter 3 index : register index\n   Parameter 4 station : equipment station number\n   Parameter 5 len : number of data read\n    Parameter 6 DataType: read data type\n   Parameter 7 array: read data\n  Parameter 8 type: type, trigger-trigger class, cycle-loop,In general, the default parameters are fine, and CYCLE is recommended if the loop script is used and the cycle time is short\n  Return：When the script type is a trigger class, the return value is the result of the acquisition, 1-success, -1-failure#7#link_0_1_0_0,reg_0_1_0_0,index_1_0_0_0,station_1_0_0_0,len_1_0_0_0,DataType_0_0_0_4,type_0_0_1_0_TRIGGER_CYCLE",
    "suspendlink@suspendlink,Pause the communication function of the specified connection on the current page\n  Parameter 1 link: connection name\n  Return：null#1#link_0_1_0_0",
    "startlink@startlink,Specify the communication function of the connection at the current page, generally used with the suspendlink function.\n  Parameter 1 link: connection name\n  Return：null#1#link_0_1_0_0",
    "switchScreen@switchScreen,Switch screen function to switch to the specified screen\n  Parameter 1 index: the screen id of the specified screen\n  Parameter 2 name: specifies the screen name of the screen\n  return：null#2#index_1_0_0_0,name_1_1_1_",
    "writedevice@writedevice,Write data to the specified connected device\nFirst form\n  Parameter 1 linkid: device connection id\n  Parameter 2 s: written string\n  Parameter 3 len: the length of the string to be written\n  Parameter 4 timeout: write timeout\n  Parameter 5 frameType: frame string format, asc, hex\nSecond form\n  Parameter 1 linkid: device connection id\n  Parameter 2 array: byte array written\n  Parameter 3 len: length of byte array written\n  Parameter 4 timeout: write timeout#5#linkid_0_0_0_0,s_1_1_0_,len_1_0_0_0,timeout(ms)_1_0_0_200,frameType_1_1_1_asc",
    "readdevice@readdevice,Read data from a specified connected device\nFirst form\n  Parameter 1 linkid: device connection id\n  Parameter 2 len: length of read\n  Parameter 3 timeout: read timeout\n  Return value: read data (string format)\nSecond form\n  Parameter 1 linkid: device connection id\n  Parameter 2 len: length of read\n  Parameter 3 timeout: read timeout\n  Parameter 4 array: read save byte array\n  return : the length of the actual read#3#linkid_0_0_0_0,len_1_0_0_0,timeout(ms)_1_0_0_200",
    "logdevice@logdevice,Turn off communication log function\n   Parameter 1 linkid: device connection id\n   Parameter 2 flag: 0 - off, 1-on#2#linkid_0_0_0_0,flag_1_0_0_0",
    "checkMethod@checkMethod,Calculate byte array and string check\nFirst form\n   Parameter 1 type: Corresponding check type crc, bcc, lrc, sum, xor\n   Parameter 2 s: string to be verified\n   Parameter 3 sType: frame format of string s, asc and hex\n   Return value: calculated check value\nSecond form\n   Parameter 1 type: Corresponding check type crc, bcc, lrc, sum, xor\n   Parameter 2 array: the byte array that needs to be evaluated\n   Parameter 3 len: the length of the corresponding byte array\n   Return value: calculated check value#3#type_1_1_0_crc,array_1_1_0_,len_1_0_0_0",
    "getdevicemutex@getdevicemutex,Get the device communication lock, if the current device communication has been blocked by the lock function\n   Parameter 1 linkid: device connection id\n   Return value: none#1#linkid_0_0_0_0",
    "releasedevicemutex@releasedevicemutex,Release device communication lock\n   Parameter 1 linkid: device connection id\n   Return value: none#1#linkid_0_0_0_0",
    "HIBYTE@HIBYTE,Take the high 8-bit data of the word\n  Parameter 1 value: data\n  Return value: the data obtained#1#value_1_0_0_0",
    "LOBYTE@LOBYTE,Take the lower 8-bit data of the word\n  Parameter 1 value: data\n  Return value: the data obtained#1#value_1_0_0_0",
    "hmiPrint@hmiPrint,Print#1#value_1_0_0_",
    "runScriptFromId@runScriptFromId,Run the specified script\n    Parameter 1 id: script id\n    Return value: none\n#1#id_1_0_0_0",
    "runScriptText@runScriptText,Run the specified script\n    Parameter 1 Text: script \n    Return value: none\n#1#Text_1_1_0_",
    "mouseClick@mouseClick,The relative coordinates of the touch screen simulate a click\n    Parameter 1 x: x coordinate \n    Parameter 2 y: y coordinate \n    Return value: none\n#2#x_1_0_0_0,y_1_0_0_0",
    "mouseClickAbs@mouseClickAbs,The absolute coordinates of the touch screen simulate a click\n    Parameter 1 x: x coordinate \n    Parameter 2 y: y coordinate \n    Return value: none\n#2#x_1_0_0_0,y_1_0_0_0"
};
const QString MACROFUNCTYPE2NAME_CN[] =
{
    QString::fromUtf8("setdata@setdata,写内外部地址的值\n第一种形态\n  参数1 link：连接的id或者连接名\n  参数2 reg：寄存器名\n  参数3 index ：寄存器索引\n  参数4 station : 设备站号\n  参数5 len : 写入长度，单位是字\n  参数6 data：写入的数据\n  参数7 DataType：写入的数据类型\n  返回值：无\n第二种形态\n  参数1 link：连接的id或者连接名\n  参数2 reg：寄存器名\n  参数3 index ：寄存器索引\n  参数 4 station : 设备站号\n  参数 5 len : 写入数据个数\n  参数 6 array：写入的数组\n  参数7 DataType：写入的数据类型\n  返回值：无 #7#link_0_1_0_0,reg_0_1_0_0,index_1_0_0_0,station_1_0_0_1,len_1_0_0_1,data_1_0_0_0,DataType_0_0_0_4"),
    QString::fromUtf8("getdata@getdata,读内外部地址的值\n第一种形态\n  参数1 link：连接的id或者连接名\n  参数2 reg：寄存器名\n  参数3 index ：寄存器索引\n  参数4 station : 设备站号\n  参数5 len : 读长度，单位是字\n  参数6 DataType：数据类型\n  参数7 type：类型，TRIGGER-触发，CYCLE-循环，一般情况下默认参数就行，如果在循环脚本中使用且循环时间较短时建议使用CYCLElE\n  返回值：当脚本类型为触发类时，函数会有两个返回值，依次是获取的值和获取结果，获取结果1-成功，-1-失败\n第二种形态\n  参数1 link：连接的id或者连接名\n  参数2 reg：寄存器名\n  参数3 index ：寄存器索引\n  参数 4 station : 设备站号\n  参数 5 len : 读的数据个数\n  参数 6 DataType：数据类型\n  参数 7 array：读到的数据\n  参数 8 type：类型，TRIGGER-触发，CYCLE-循环，一般情况下默认参数就行，如果在循环脚本中使用且循环时间较短时建议使用CYCLElE\n  返回值：当脚本类型为触发类时，返回值为获取结果，1-成功，-1-失败 #7#link_0_1_0_0,reg_0_1_0_0,index_1_0_0_0,station_1_0_0_0,len_1_0_0_0,DataType_0_0_0_4,type_0_0_1_0_TRIGGER_CYCLE"),
    QString::fromUtf8("suspendlink@suspendlink,在当前页面暂停指定连接的通信功能\n  参数1 link：连接名\n  返回值：无#1#link_0_1_0_0"),
    QString::fromUtf8("startlink@startlink,在当前页面开始指定连接的通信功能，一般和suspendlink函数配套使用\n  参数1 link：连接名\n  返回值：无#1#link_0_1_0_0"),
    QString::fromUtf8("switchScreen@switchScreen,切换画面函数，切换到指定的画面\n  参数1 index：指定画面的画面id\n  参数2 name：指定画面的画面名\n  返回值：无#2#index_1_0_0_0,name_1_1_1_"),
    QString::fromUtf8("writedevice@writedevice,向指定的连接设备写入数据\n第一种形态\n  参数1 linkid：设备连接id\n  参数2 s：写入的字符串\n  参数3 len：写入的字符串长度\n  参数4 timeout：写入超时时间\n  参数5 frameType：帧字符串格式，asc，hex\n第二种形态\n  参数1 linkid：设备连接id\n  参数2 array：写入的字节数组\n  参数3 len：写入的字节数组长度\n  参数4 timeout：写入超时时间#5#linkid_0_0_0_0,s_1_1_0_,len_1_0_0_0,timeout(ms)_1_0_0_200,frameType_1_1_1_asc"),
    QString::fromUtf8("readdevice@readdevice,从指定的连接设备读取数据\n第一种形态\n  参数1 linkid：设备连接id\n  参数2 len：读的长度\n  参数3 timeout：读取超时时间\n  返回值：读取到的数据（字符串格式）\n第二种形态\n  参数1 linkid：设备连接id\n  参数2 len：读的长度\n  参数3 timeout：读取超时时间\n  参数4 array：读保存字节数组\n  返回值：实际读取的长度#3#linkid_0_0_0_0,len_1_0_0_0,timeout(ms)_1_0_0_200"),
    QString::fromUtf8("getdevicemutex@getdevicemutex,获取设备通信锁，如果当前设备通信已被锁函数会阻塞\n  参数1 linkid：设备连接id\n  返回值：无#1#linkid_0_0_0_0"),
    QString::fromUtf8("releasedevicemutex@releasedevicemutex,释放设备通信锁\n  参数1 linkid：设备连接id\n  返回值：无#1#linkid_0_0_0_0"),
    QString::fromUtf8("logdevice@logdevice,开启关闭通讯日志功能\n  参数1 linkid：设备连接id\n  参数2 flag：0-关闭，1-打开#2#linkid_0_0_0_0,flag_1_0_0_0"),
    QString::fromUtf8("checkMethod@checkMethod,计算字节数组和字符串校验\n第一种形态\n  参数1 type：对应的校验类型crc，bcc，lrc，sum，xor\n  参数2 s：需要计算校验的字符串\n  参数3 sType：字符串s的帧格式，asc和hex\n  返回值：计算的校验值\n第二种形态\n  参数1 type：对应的校验类型crc，bcc，lrc，sum，xor\n  参数2 array：需要计算校验的字节数组\n  参数3 len：对应字节数组的长度\n  返回值：计算的校验值#3#type_1_1_0_crc,array_1_1_0_,len_1_0_0_0"),
    QString::fromUtf8("HIBYTE@HIBYTE,取字的高8位数据\n  参数1 value：数据\n  返回值：得到的数据#1#value_1_0_0_0"),
    QString::fromUtf8("LOBYTE@LOBYTE,取字的低8位数据\n  参数1 value：数据\n  返回值：得到的数据#1#value_1_0_0_0"),
    QString::fromUtf8("hmiPrint@hmiPrint,打印函数#1#value_1_0_0_"),
    QString::fromUtf8("runScriptFromId@runScriptFromId,运行指定脚本\n   参数1 id：脚本id\n   返回值：无#1#id_1_0_0_0"),
    QString::fromUtf8("runScriptText@runScriptText,运行指定脚本\n   参数1 Text：脚本\n   返回值：无#1#Text_1_1_0_"),
    QString::fromUtf8("mouseClick@mouseClick,触摸屏相对坐标模拟点击\n   参数1 x：x坐标\n   参数2 y：y坐标\n   返回值：无#2#x_1_0_0_0,y_1_0_0_0"),
    QString::fromUtf8("mouseClickAbs@mouseClickAbs,触摸屏绝对坐标模拟点击\n   参数1 x：x坐标\n   参数2 y：y坐标\n   返回值：无#2#x_1_0_0_0,y_1_0_0_0")
};/*bug3161  脚本中，操作组态工程运行 readdevice函数的介绍中的，第二种形态的参数二应该放在最后_yangjindong_20180921_end*/

////3.基本库函数
const int MACROFUNCTYPE3NUM = 91;
const int MACROFUNCTYPE3_1NUM = 10;
const int MACROFUNCTYPE3_2NUM = 17;
const int MACROFUNCTYPE3_3NUM = 34;
const int MACROFUNCTYPE3_4NUM = 57;
const int MACROFUNCTYPE3_5NUM = 74;
const int MACROFUNCTYPE3_6NUM = 85;

const QString MACROFUNCTYPE3NAME_CN[] =
{
    QString::fromUtf8("os.execute@os.execute ([command])\n  这个函数等价于 ISO C 函数 system。 它调用系统解释器执行 command。 如果命令成功运行完毕，第一个返回值就是 true， 否则是 nil otherwise。 在第一个返回值之后，函数返回一个字符串加一个数字。如下：\n'exit': 命令正常结束； 接下来的数字是命令的退出状态码。\n'signal': 命令被信号打断； 接下来的数字是打断该命令的信号。\n如果不带参数调用， os.execute 在系统解释器存在的时候返回真。#1#command_1_1_0_"),
    QString::fromUtf8("os.exit@os.exit ([code [, close]])\n  调用 ISO C 函数 exit 终止宿主程序。 如果 code 为 true， 返回的状态码是 EXIT_SUCCESS； 如果 code 为 false， 返回的状态码是 EXIT_FAILURE； 如果 code 是一个数字， 返回的状态码就是这个数字。 code 的默认值为 true。如果第二个可选参数 close 为真， 在退出前关闭 Lua 状态机。#2#a_1_0_0_1,b_1_0_0_1"),
    QString::fromUtf8("os.clock@os.clock ()\n  返回程序使用的按秒计 CPU 时间的近似值。#0#"),
    QString::fromUtf8("os.date@os.date ([format [, time]])\n  返回一个包含日期及时刻的字符串或表。 格式化方法取决于所给字符串 format。如果提供了 time 参数， 格式化这个时间 （这个值的含义参见 os.time 函数）。 否则，date 格式化当前时间。如果 format 以 '!' 打头， 日期以协调世界时格式化。 在这个可选字符项之后， 如果 format 为字符串 '*t'， date 返回有后续域的表： year （四位数字），month （1–12），day （1–31）， hour （0–23），min （0–59），sec （0–61）， wday （星期几，星期天为 1 ）， yday （当年的第几天）， 以及 isdst （夏令时标记，一个布尔量）。 对于最后一个域，如果该信息不提供的话就不存在。如果 format 并非 '*t'， date 以字符串形式返回， 格式化方法遵循 ISO C 函数 strftime 的规则。如果不传参数调用， date 返回一个合理的日期时间串， 格式取决于宿主程序以及当前的区域设置 （即，os.date() 等价于 os.date('%c'')）。在非 POSIX 系统上， 由于这个函数依赖 C 函数 gmtime 和 localtime， 它可能并非线程安全的。#2#format_1_1_1_,time_1_0_1_0"),
    QString::fromUtf8("os.difftime@os.difftime (t2, t1)\n  返回以秒计算的时刻 t1 到 t2 的差值。 （这里的时刻是由 os.time 返回的值）。 在 POSIX，Windows，和其它一些系统中，这个值就等于 t2-t1。#2#t1_1_0_1_0,t2_1_0_1_0"),
    QString::fromUtf8("os.getenv@os.getenv (varname)\n  返回进程环境变量 varname 的值， 如果该变量未定义，返回 nil 。#1#varname_1_1_0_"),
    QString::fromUtf8("os.remove@os.remove (filename)\n  删除指定名字的文件（在 POSIX 系统上可以是一个空目录） 如果函数失败，返回 nil 加一个错误描述串及出错码。#1#filename_1_1_0_"),
    QString::fromUtf8("os.rename@os.rename (oldname, newname)\n  将名字为 oldname 的文件或目录更名为 newname。 如果函数失败，返回 nil 加一个错误描述串及出错码。#2#oldname_1_1_0_,newname_1_1_0_"),
    QString::fromUtf8("os.setlocale@os.setlocale (locale [, category])\n  设置程序的当前区域。 locale 是一个区域设置的系统相关字符串； category 是一个描述有改变哪个分类的可选字符串： 'all'，'collate'， 'ctype'， 'monetary'， 'numeric'， 或 'time'； 默认的分类为 'all'。 此函数返回新区域的名字。 如果请求未被获准，返回 nil 。当 locale 是一个空串， 当前区域被设置为一个在实现中定义好的本地区域。 当 locale 为字符串 'C'， 当前区域被设置为标准 C 区域。当第一个参数为 nil 时， 此函数仅返回当前区域指定分类的名字。由于这个函数依赖 C 函数 setlocale， 它可能并非线程安全的。#2#locale_1_1_0_,category_0_1_1_all_collate_ctype_monetary_numeric_time"),
    QString::fromUtf8("os.time@os.time ([table])\n  当不传参数时，返回当前时刻。 如果传入一张表，就返回由这张表表示的时刻。 这张表必须包含域 year，month，及 day； 可以包含有　hour （默认为 12 ）， min （默认为 0）， sec （默认为 0），以及 isdst （默认为 nil）。 关于这些域的详细描述，参见 os.date 函数。返回值是一个含义由你的系统决定的数字。 在 POSIX，Windows，和其它一些系统中， 这个数字统计了从指定时间（'epoch'）开始经历的秒数。 对于另外的系统，其含义未定义， 你只能把 time 的返回数字用于 os.date 和 os.difftime 的参数。#1#table_1_0_1_"),
    QString::fromUtf8("os.tmpname@os.tmpname ()\n  返回一个可用于临时文件的文件名字符串。 这个文件在使用前必须显式打开，不再使用时需要显式删除。在 POSIX 系统上， 这个函数会以此文件名创建一个文件以回避安全风险。 （别人可能未经允许在获取到这个文件名到创建该文件之间的时刻创建此文件。） 你依旧需要在使用它的时候先打开，并最后删除（即使你没使用到）。只有有可能，你更应该使用 io.tmpfile， 因为该文件可以在程序结束时自动删除。#0#"),///10

    QString::fromUtf8("coroutine.create@coroutine.create (f)\n  创建一个主体函数为 f 的新协程。 f 必须是一个 Lua 的函数。 返回这个新协程，它是一个类型为 'thread' 的对象。#1#threadFunc_1_0_0_f"),///11
    QString::fromUtf8("coroutine.isyieldable@coroutine.isyieldable ()\n  如果正在运行的协程可以让出，则返回真。不在主线程中或不在一个无法让出的 C 函数中时，当前协程是可让出的。#0#"),
    QString::fromUtf8("coroutine.resume@coroutine.resume (co [, val1, ···])\n  开始或继续协程 co 的运行。 当你第一次延续一个协程，它会从主体函数处开始运行。 val1, ... 这些值会以参数形式传入主体函数。 如果该协程被让出，resume 会重新启动它； val1, ... 这些参数会作为让出点的返回值。如果协程运行起来没有错误， resume 返回 true 加上传给 yield 的所有值 （当协程让出）， 或是主体函数的所有返回值（当协程中止）。 如果有任何错误发生， resume 返回 false 加错误消息。#3#co_1_0_0_co,val1_1_0_1_val1,uncertain_1_0_2_..."),
    QString::fromUtf8("coroutine.running@coroutine.running ()\n  返回当前正在运行的协程加一个布尔量。 如果当前运行的协程是主线程，其为真。#0#"),
    QString::fromUtf8("coroutine.status@coroutine.status (co)\n  以字符串形式返回协程 co 的状态： 当协程正在运行（它就是调用 status 的那个） ，返回 'running'； 如果协程调用 yield 挂起或是还没有开始运行，返回 'suspended'； 如果协程是活动的，都并不在运行（即它正在延续其它协程），返回 'normal'； 如果协程运行完主体函数或因错误停止，返回 'dead'。#1#co_1_0_0_co"),
    QString::fromUtf8("coroutine.wrap@coroutine.wrap (f)\n  创建一个主体函数为 f 的新协程。 f 必须是一个 Lua 的函数。 返回一个函数， 每次调用该函数都会延续该协程。 传给这个函数的参数都会作为 resume 的额外参数。 和 resume 返回相同的值， 只是没有第一个布尔量。 如果发生任何错误，抛出这个错误.#1#threadFunc_1_0_0_f"),
    QString::fromUtf8("coroutine.yield@coroutine.yield (...)\n  挂起正在调用的协程的执行。 传递给 yield 的参数都会转为 resume 的额外返回值。#1#uncertain_1_0_2_..."),

    QString::fromUtf8("io.close@io.close ([file])\n  等价于 file:close()。 不给出 file 时将关闭默认输出文件。#1#file_1_0_1_"),
    QString::fromUtf8("io.flush@io.flush ()\n  等价于 io.output():flush()#0#"),
    QString::fromUtf8("io.input@io.input ([file])\n  用文件名调用它时，（以文本模式）来打开该名字的文件， 并将文件句柄设为默认输入文件。 如果用文件句柄去调用它， 就简单的将该句柄设为默认输入文件。 如果调用时不传参数，它返回当前的默认输入文件。#1#file_1_0_1_"),
    QString::fromUtf8("io.lines@io.lines ([filename ...])\n  以读模式打开指定的文件名并返回一个迭代函数。 此迭代函数的工作方式和用一个已打开的文件去调用 file:lines(···) 得到的迭代器相同。 当迭代函数检测到文件结束， 它不返回值（让循环结束）并自动关闭文件.#2#filename_1_1_1_,uncertain_1_0_2_..."),
    QString::fromUtf8("io.open@io.open (filename [, mode])\n  这个函数用字符串 mode 指定的模式打开一个文件。 返回新的文件句柄。 当出错时，返回 nil 加错误消息。mode 字符串可以是下列任意值：'r': 读模式（默认）；'w': 写模式；'a': 追加模式；'r+': 更新模式，所有之前的数据都保留；'w+': 更新模式，所有之前的数据都删除；'a+': 追加更新模式，所有之前的数据都保留，只允许在文件尾部做写入。mode 字符串可以在最后加一个 'b' ， 这会在某些系统上以二进制方式打开文件。#2#filename_1_1_1_,mode_0_1_1_0_r_w_a_r+_w+_a+"),
    QString::fromUtf8("io.output@io.output ([file])\n  类似于 io.input。 不过都针对默认输出文件操作。#1#file_1_0_1_"),
    QString::fromUtf8("io.read@io.read (...)\n  等价于 io.input():read(···)。#1#uncertain_1_0_2_..."),
    QString::fromUtf8("io.tmpfile@io.tmpfile ()\n  返回一个临时文件的句柄。 这个文件以更新模式打开，在程序结束时会自动删除。#0#"),
    QString::fromUtf8("io.type@io.type (obj)\n  检查 obj 是否是合法的文件句柄。 如果 obj 它是一个打开的文件句柄，返回字符串 'file'。 如果 obj 是一个关闭的文件句柄，返回字符串 'closed file'。 如果 obj 不是文件句柄，返回 nil 。#1#obj_1_0_0_"),
    QString::fromUtf8("io.write@io.write (...)\n  等价于 io.output():write(···)。#1#uncertain_1_0_2_..."),
    QString::fromUtf8("file:close@file:close ()\n  关闭 file。 注意，文件在句柄被垃圾回收时会自动关闭， 但是多久以后发生，时间不可预期的。当关闭用 io.popen 创建出来的文件句柄时， file:close 返回 os.execute 会返回的一样的值。#0#"),
    QString::fromUtf8("file:flush@file:flush ()\n  将写入的数据保存到 file 中。#0#"),
    QString::fromUtf8("file:lines@file:lines (...)\n  返回一个迭代器函数， 每次调用迭代器时，都从文件中按指定格式读数据。 如果没有指定格式，使用默认值 'l'' 。 看一个例子for c in file:lines(1) do body end会从文件当前位置开始，中不断读出字符。 和 io.lines 不同， 这个函数在循环结束后不会关闭文件。在出错的情况下，函数抛出错误而不是返回错误码。#1#uncertain_1_0_2_..."),
    QString::fromUtf8("file:read@file:read (...)\n  读文件 file， 指定的格式决定了要读什么。 对于每种格式，函数返回读出的字符对应的字符串或数字。 若不能以该格式对应读出数据则返回 nil。 （对于最后这种情况， 函数不会读出后续的格式。） 当调用时不传格式，它会使用默认格式读下一行（见下面描述）。提供的格式有'n': 读取一个数字，根据 Lua 的转换文法，可能返回浮点数或整数。 （数字可以有前置或后置的空格，以及符号。） 只要能构成合法的数字，这个格式总是去读尽量长的串； 如果读出来的前缀无法构成合法的数字 （比如空串，'0x'' 或 '3.4e-'）， 就中止函数运行，返回 nil。'i': 读取一个整数，返回整数值。'a': 从当前位置开始读取整个文件。 如果已在文件末尾，返回空串。'l': 读取一行并忽略行结束标记。 当在文件末尾时，返回 nil 这是默认格式。'L': 读取一行并保留行结束标记（如果有的话）， 当在文件末尾时，返回 nil。number: 读取一个不超过这个数量字节数的字符串。 当在文件末尾时，返回 nil。 如果 number 为零， 它什么也不读，返回一个空串。 当在文件末尾时，返回 nil。格式 'l' 和 'L' 只能用于文本文件。#1#uncertain_1_0_2_..."),
    QString::fromUtf8("file:seek@file:seek ([whence [, offset]])\n  设置及获取基于文件开头处计算出的位置。 设置的位置由 offset 和 whence 字符串 whence 指定的基点决定。基点可以是：'set': 基点为 0 （文件开头）；'cur': 基点为当前位置了；'end': 基点为文件尾；当 seek 成功时，返回最终从文件开头计算起的文件的位置。 当 seek 失败时，返回 nil 加上一个错误描述字符串。whence 的默认值是 'cur'， offset 默认为 0 。 因此，调用 file:seek() 可以返回文件当前位置，并不改变它； 调用 file:seek('set') 将位置设为文件开头（并返回 0）； 调用 file:seek('end'') 将位置设到文件末尾，并返回文件大小。#2#where_0_1_0_0_cur_set_end,offset _1_0_0_0"),
    QString::fromUtf8("file:setvbuf@file:setvbuf (mode [, size])\n  设置输出文件的缓冲模式。 有三种模式：'no': 不缓冲；输出操作立刻生效。'full': 完全缓冲；只有在缓存满或当你显式的对文件调用 flush（参见 io.flush） 时才真正做输出操作。'line': 行缓冲； 输出将到每次换行前， 对于某些特殊文件（例如终端设备）缓冲到任何输入前。对于后两种情况，size 以字节数为单位 指定缓冲区大小。 默认会有一个恰当的大小。#2#mode_0_1_1_0_no_full_line,size _1_0_1_0"),
    QString::fromUtf8("file:write@file:write (...)\n  将参数的值逐个写入 file。 参数必须是字符串或数字。成功时，函数返回 file。 否则返回 nil 加错误描述字符串。#1#uncertain_1_0_2_..."),

    QString::fromUtf8("math.abs@math.abs (x)\n  返回 x 的绝对值。(integer/float)#1#x_1_0_0_0"),
    QString::fromUtf8("math.acos@math.acos (x)\n  返回 x 的反余弦值（用弧度表示）。#1#x_1_0_0_0"),
    QString::fromUtf8("math.asin@math.asin (x)\n  返回 x 的反正弦值（用弧度表示）。#1#x_1_0_0_0"),
    QString::fromUtf8("math.atan@math.atan (y [, x])\n  返回 y/x 的反正切值（用弧度表示）。 它会使用两个参数的符号来找到结果落在哪个象限中。 （即使 x 为零时，也可以正确的处理。）默认的 x 是 1 ， 因此调用 math.atan(y) 将返回 y 的反正切值。#2#y_1_0_0_0,x_1_0_1_1"),
    QString::fromUtf8("math.ceil@math.ceil (x)\n  返回不小于 x 的最小整数值。#1#x_1_0_0_0"),
    QString::fromUtf8("math.cos@math.cos (x)\n  返回 x 的余弦（假定参数是弧度）。#1#x_1_0_0_0"),
    QString::fromUtf8("math.deg@math.deg (x)\n  将角 x 从弧度转换为角度。#1#x_1_0_0_0"),
    QString::fromUtf8("math.exp@math.exp (x)\n  返回 ex 的值 （e 为自然对数的底）#1#x_1_0_0_0"),
    QString::fromUtf8("math.floor@math.floor (x)\n  返回不大于 x 的最大整数值。#1#x_1_0_0_0"),
    QString::fromUtf8("math.fmod@math.fmod (x, y)\n  返回 x 除以 y，将商向零圆整后的余数。 (integer/float)#1#x_1_0_0_0,x_1_0_0_0"),
    QString::fromUtf8("math.log@math.log (x [, base])\n  返回以指定底的 x 的对数。 默认的 base 是 e （因此此函数返回 x 的自然对数）。#2#x_1_0_0_0,base_1_0_1_e"),
    QString::fromUtf8("math.max@math.max (x, ...)\n  返回参数中最大的值， 大小由 Lua 操作 < 决定。 (integer/float)#2#x_1_0_0_0,uncertain_1_0_2_..."),
    QString::fromUtf8("math.min@math.min (x, ...)\n  返回参数中最小的值， 大小由 Lua 操作 < 决定。 (integer/float)#2#x_1_0_0_0,uncertain_1_0_2_..."),
    QString::fromUtf8("math.modf@math.modf (x)\n  返回 x 的整数部分和小数部分。 第二个结果一定是浮点数。#1#x_1_0_0_0"),
    QString::fromUtf8("math.rad@math.rad (x)\n  将角 x 从角度转换为弧度。#1#x_1_0_0_0"),
    QString::fromUtf8("math.random@math.random ([m [, n]])\n  当不带参数调用时， 返回一个 [0,1) 区间内一致分布的浮点伪随机数。 当以两个整数 m 与 n 调用时， math.random 返回一个 [m, n] 区间 内一致分布的整数伪随机数。 （值 m-n 不能是负数，且必须在 Lua 整数的表示范围内。） 调用 math.random(n) 等价于 math.random(1,n)。这个函数是对 C 提供的位随机数函数的封装。 对其统计属性不作担保。#2#m_1_0_1_0,n_1_0_1_1"),
    QString::fromUtf8("math.randomseed@math.randomseed (x)\n  把 x 设为伪随机数发生器的“种子”： 相同的种子产生相同的随机数列。#1#x_1_0_0_0"),
    QString::fromUtf8("math.sin@math.sin (x)\n  返回 x 的正弦值（假定参数是弧度）。#1#x_1_0_0_0"),
    QString::fromUtf8("math.sqrt@math.sqrt (x)\n 返回 x 的平方根。 （你也可以使用乘方 x^0.5 来计算这个值。）#1#x_1_0_0_0"),
    QString::fromUtf8("math.tan@math.tan (x)\n  返回 x 的正切值（假定参数是弧度）。#1#x_1_0_0_0"),
    QString::fromUtf8("math.tointeger@math.tointeger (x)\n  如果 x 可以转换为一个整数， 返回该整数。 否则返回 nil。#1#x_1_0_0_0"),
    QString::fromUtf8("math.type@math.type (x)\n  如果 x 是整数，返回 'integer'， 如果它是浮点数，返回 'float'， 如果 x 不是数字，返回 nil。#1#x_1_0_0_0"),
    QString::fromUtf8("math.ult@math.ult (m, n)\n  如果整数 m 和 n 以无符号整数形式比较， m 在 n 之下，返回布尔真否则返回假。#1#m_1_0_0_0,n_1_0_0_0"),

    QString::fromUtf8("string.byte@string.byte (s [, i [, j]])\n  返回字符 s[i]， s[i+1]， ...　，s[j] 的内部数字编码。 i 的默认值是 1 ； j 的默认值是 i。 这些索引以函数 string.sub 的规则修正。数字编码没有必要跨平台。#3#s_1_1_0_,i_1_0_1_1,j_1_0_1_1"),////58
    QString::fromUtf8("string.char@string.char (...)\n  接收零或更多的整数。 返回和参数数量相同长度的字符串。 其中每个字符的内部编码值等于对应的参数值。数字编码没有必要跨平台。#1#uncertain_1_0_2_..."),
    QString::fromUtf8("string.dump@string.dump (function [, strip])\n  返回包含有以二进制方式表示的（一个 二进制代码块 ）指定函数的字符串。 之后可以用 load 调用这个字符串获得 该函数的副本（但是绑定新的上值）。 如果　strip 为真值， 二进制代码块不携带该函数的调试信息 （局部变量名，行号，等等。）。带上值的函数只保存上值的数目。 当（再次）加载时，这些上值被更新为 nil 的实例。 （你可以使用调试库按你需要的方式来序列化上值，并重载到函数中）#2#function_1_0_1_func,strip_1_0_1_0"),
    QString::fromUtf8("string.find@string.find (s, pattern [, init [, plain]])\n  查找第一个字符串 s 中匹配到的 pattern （参见 §6.4.1）。 如果找到一个匹配，find 会返回 s 中关于它起始及终点位置的索引； 否则，返回 nil。 第三个可选数字参数 init 指明从哪里开始搜索； 默认值为 1 ，同时可以是负值。 第四个可选参数 plain 为 true 时， 关闭模式匹配机制。 此时函数仅做直接的 “查找子串”的操作， 而 pattern 中没有字符被看作魔法字符。 注意，如果给定了 plain　，就必须写上 init 。如果在模式中定义了捕获，捕获到的若干值也会在两个索引之后返回。#4#s_1_1_0_,pattern_1_1_0_,init_1_0_1_1,plain_1_0_1_0"),
    QString::fromUtf8("string.format@string.format (formatstring, ...)\n  返回不定数量参数的格式化版本， 格式化串为第一个参数（必须是一个字符串）。 格式化字符串遵循 ISO C 函数 sprintf 的规则。 不同点在于选项 *, h, L, l, n, p 不支持， 另外还增加了一个选项 q。 q 选项将一个字符串格式化为两个双引号括起，对内部字符做恰当的转义处理的字符串。 该字符串可以安全的被 Lua 解释器读回来。#2#formatstring_1_1_0_,uncertain_1_0_2_..."),
    QString::fromUtf8("string.gmatch@string.gmatch (s, pattern)\n  返回一个迭代器函数。 每次调用这个函数都会继续以 pattern （参见　§6.4.1） 对 s 做匹配，并返回所有捕获到的值。 如果 pattern 中没有指定捕获，则每次捕获整个 pattern。#2#s_1_1_0_,pattern_1_1_0_"),
    QString::fromUtf8("string.gsub@string.gsub (s, pattern, repl [, n])\n  将字符串 s 中，所有的（或是在 n 给出时的前 n 个） pattern （参见 §6.4.1）都替换成 repl ，并返回其副本。 repl 可以是字符串、表、或函数。 gsub 还会在第二个返回值返回一共发生了多少次匹配。 gsub 这个名字来源于 Global SUBstitution 。如果 repl 是一个字符串，那么把这个字符串作为替换品。 字符 % 是一个转义符： repl 中的所有形式为 %d 的串表示 第 d 个捕获到的子串，d 可以是 1 到 9 。 串 %0 表示整个匹配。 串 %% 表示单个 %。如果 repl 是张表，每次匹配时都会用第一个捕获物作为键去查这张表。如果 repl 是个函数，则在每次匹配发生时都会调用这个函数。 所有捕获到的子串依次作为参数传入。任何情况下，模板中没有设定捕获都看成是捕获整个模板。如果表的查询结果或函数的返回结果是一个字符串或是个数字， 都将其作为替换用串； 而在返回 false 或 nil　时不作替换 （即保留匹配前的原始串）。#4#s_1_1_0_,pattern_1_1_0_,repl_1_0_0_,n_1_0_1_0"),
    QString::fromUtf8("string.len@string.len (s)\n  接收一个字符串，返回其长度。 空串 "" 的长度为 0 。 内嵌零也统计在内。#1#s_1_1_0_"),
    QString::fromUtf8("string.lower@string.lower (s)\n  接收一个字符串，将其中的大写字符都转为小写后返回其副本。 其它的字符串不会更改。 对大写字符的定义取决于当前的区域设置。#1#s_1_1_0_"),
    QString::fromUtf8("string.match@string.match (s, pattern [, init])\n  在字符串 s 中找到第一个能用 pattern （参见 §6.4.1）匹配到的部分。 如果能找到，match 返回其中的捕获物； 否则返回 nil 。 如果 pattern 中未指定捕获， 返回整个 pattern 捕获到的串。 第三个可选数字参数 init 指明从哪里开始搜索； 它默认为 1 且可以是负数。#3#s_1_1_0_,pattern_1_1_0_,init_1_0_1_1"),
    QString::fromUtf8("string.pack@string.pack (fmt, v1, v2, ...)\n  返回一个打包了（即以二进制形式序列化） v1, v2 等值的二进制字符串。 字符串 fmt 为打包格式（参见 §6.4.2）。#4#fmt_1_1_0_,v1_1_0_0_v1,v2_1_0_0_v2,uncertain_1_0_2_..."),
    QString::fromUtf8("string.packsize@string.packsize (fmt)\n  返回以指定格式用 string.pack 打包的字符串的长度。 格式化字符串中不可以有变长选项 's' 或 'z' （参见 §6.4.2）。#1#fmt_1_1_0_"),
    QString::fromUtf8("string.rep@string.rep (s, n [, sep])\n  返回 n 个字符串 s 以字符串 sep 为分割符连在一起的字符串。 默认的 sep 值为空字符串（即没有分割符）。 如果 n 不是正数则返回空串。#3#s_1_1_0_,n_1_0_0_1,sep_1_1_1_"),
    QString::fromUtf8("string.reverse@string.返回字符串 s 的翻转串。#1#s_1_1_0_"),
    QString::fromUtf8("string.sub@string.sub (s, i [, j])\n  返回 s 的子串， 该子串从 i 开始到 j 为止； i 和 j 都可以为负数。 如果不给出 j ，就当它是 -1 （和字符串长度相同）。 特别是， 调用 string.sub(s,1,j) 可以返回 s 的长度为 j 的前缀串， 而 string.sub(s, -i) 返回长度为 i 的后缀串。如果在对负数索引转义后 i 小于 1 的话，就修正回 1 。 如果 j 比字符串的长度还大，就修正为字符串长度。 如果在修正之后，i 大于 j， 函数返回空串。#3#s_1_1_0_,i_1_0_0_0,j_1_0_0_0"),
    QString::fromUtf8("string.unpack@string.unpack (fmt, s [, pos])\n  返回以格式 fmt （参见 §6.4.2） 打包在字符串 s （参见 string.pack） 中的值。 选项 pos（默认为 1 ）标记了从 s 中哪里开始读起。 读完所有的值后，函数返回 s 中第一个未读字节的位置.#3#fmt_1_1_0_,s_1_1_0_,pos_1_0_1_1"),
    QString::fromUtf8("string.upper@string.upper (s)\n  接收一个字符串，将其中的小写字符都转为大写后返回其副本。 其它的字符串不会更改。 对小写字符的定义取决于当前的区域设置。#1#s_1_1_0_"),

    QString::fromUtf8("timerRun@timerRun(id),启动指定id的系统定时器\n   参数1 id：系统定时器id号，0-35#1#id_1_0_0_0"),
    QString::fromUtf8("timerStop@timerStop(id),停止指定id的系统定时器\n   参数1 id：系统定时器id号，0-35#1#id_1_0_0_0"),
    QString::fromUtf8("timerReset@timerReset(id,value),设置定时器的当前值，由第二个参数设定\n  参数1 id：系统定时器id号，0-35 \n  参数2 value：设定的值\n  返回值：操作结果0-失败，1-成功#2#id_1_0_0_0,value_1_0_0_0"),
    QString::fromUtf8("timerSkip@timerSkip(id,value),在指定定时器当前时间数上加/减指定值\n  参数1 id：系统定时器id号，0-35 \n  参数2 value：加减的指定值\n  返回值：操作结果0-失败，1-成功#2#id_1_0_0_0,value_1_0_0_0"),
    QString::fromUtf8("timerValue@timerValue(id),取指定定时器的当前值\n  参数1 id：系统定时器id号，0-35 \n  返回值：取到的数据#1#id_1_0_0_0"),
    QString::fromUtf8("timerStr@timerStr(id,type),以字符串的形式返回指定定时器的当前值\n  参数1 id：系统定时器id号，0-35\n  参数2 type：字符串格式\n  返回值：取到的字符串#2#id_1_0_0_0,type_1_0_0_0"),
    QString::fromUtf8("timerState@timerState(id),取指定定时器的工作状态\n  参数1 id：系统定时器id号，0-35\n  返回值：工作状态，0-定时器停止，1-定时器运行中#1#id_1_0_0_0"),
    QString::fromUtf8("timerSetLimit@timerSetLimit(id,num,type),设置指定定时器的最大值，即设置定时器的上限\n  参数1 id：系统定时器id号，0-35\n  参数2 num：最大值，整型数据\n  参数3 type：定时器达到上限时的动作类型，1 - 表示运行上限到后停止；0 - 表示运行到上限后重新循环运行\n  返回值：操作结果0-失败，1-成功#3#id_1_0_0_0,value_1_0_0_0,type_1_0_0_0"),
    QString::fromUtf8("timerSetOutput@timerSetOutput(id,linkid,reg,index,station),设置指定定时器的值输出地址\n  参数1 id：系统定时器id号，0-35\n  参数2 link：连接的id或者连接名\n  参数3 reg：寄存器名\n  参数4 index ：寄存器索引\n  参数5 station : 设备站号\n  返回值：操作结果0-失败，1-成功#5#id_1_0_0_0,linkid_0_0_0_0,reg_0_1_0_0,index_1_0_0_0,station_1_0_0_0"),
    QString::fromUtf8("timerWaitFor@timerWaitFor(value),等到指定时间之后向下执行\n  参数1 value：指定的值，整型数据，ms\n  返回值：无#1#value_1_0_0_0"),
    QString::fromUtf8("timerOutRun@timerOutRun(id,id),指定定时器达到上限值时运行指定脚本\n  参数1 id：系统定时器id号，0-35\n  参数2 id：指定的脚本id\n  返回值：操作结果0-失败，1-成功#2#id_1_0_0_0,id_1_0_0_0"),

    QString::fromUtf8("initCommunication@initCommunication(type,par),初始化通信接口\n  参数1 type：0-串口，1-网口\n  参数2 par：通信参数，字符串类型\n             串口时例如1-9600-8-1-NONE，第一段com号，第二段波特率，第三段数据位，第四段停止位，第五段校验\n             网口时例如1-0-192.168.1.100-8000，第一段0-udp1-tcp，第二段0-客户端1-服务端，第三段代表ip地址，第四段代表端口\n  返回值：通信id#2#type_1_0_0_0,par_1_1_0_1-9600-8-1-NONE"),
    QString::fromUtf8("sendData@sendData(),通过通信id发送数据\n第一种形态\n  参数1 id：通信id\n  参数2 s：写入的字符串\n  参数3 len：写入的字符串长度\n  参数4 timeout：写入超时时间\n  参数5 frameType：帧字符串格式，asc，hex\n  参数6 clientIdOrUdpPort：创建通讯为tcp服务端时是已连接的客户端id，udp通信时是远端端口号\n  参数7 udpIp：通讯为Udp时是远端ip地址，否则此参数无意义\n  返回值：实际发送的长度，小于0时通信错误\n第二种形态\n  参数1 id：通信id\n  参数2 array：写入的字节数组\n  参数3 len：写入的字节数组长度\n  参数4 timeout：写入超时时间\n  参数5 clientIdOrUdpPort：创建通讯为tcp服务端时是已连接的客户端id，udp通信时是远端端口号\n  参数6 udpIp：通讯为Udp时是远端ip地址，否则此参数无意义\n  返回值：实际发送的长度，小于0时通信错误#7#id_1_0_0_0,s_1_1_0_,len_1_0_0_0,timeout(ms)_1_0_0_200,frameType_1_1_1_asc,clientId_1_0_1_0,udpIp_1_1_1_0.0.0.0"),
    QString::fromUtf8("recvData@recvData,通过通信id读取数据\n第一种形态\n  参数1 id：通信id\n  参数3 len：读取长度\n  参数4 timeout：读取超时时间\n  参数5 clientId：当创建通讯为tcp服务端时已连接的客户端id\n  返回值：接收的数据字符串的格式返回\n第二种形态\n  参数1 id：通信id\n  参数2 array：读取的字节数组\n  参数3 len：读取长度\n  参数4 timeout：读取超时时间\n  参数5 clientId：当创建通讯为tcp服务端时已连接的客户端id\n  返回值：实际读取的长度，小与0时通信错误#4#id_1_0_0_0,len_1_0_0_0,delaytime(ms)_1_0_0_200,clientId_1_0_1_0"),
    QString::fromUtf8("getLinkNum@getLinkNum,tcp服务端获取已连接客户端的个数\n  参数1 id：通信id\n  返回值：已连接客户端的个数#1#id_1_0_0_0"),
    QString::fromUtf8("closeCommunication@closeCommunication,关闭指定id通信功能\n  参数1 id：通信id\n  返回值：无#1#id_1_0_0_0")
};

const QString MACROFUNCTYPE3NAME_EN[] =
{
    "os.execute@os.execute ([command])\n  This function is equivalent to the ISO C function system.It calls the system interpreter to execute command.If the command completes successfully, the first return value is true, otherwise nil otherwise.After the first return value, the function returns a string plus a number.As follows:'Exit ': the command completes normally;The next number is the exit status code of the command.'Signal ': the command is interrupted by a signal;The next number is a signal to interrupt the command.If you do not call with arguments, OS. Execute returns true when the system interpreter exists.#1#command_1_1_0_",///0
    "os.exit@os.exit ([code [, close]])\n  Call the ISO C function exit to terminate the host program.If code is true, the status code returned is EXIT_SUCCESS;If code is false, the status code returned is EXIT_FAILURE;If code is a number, the return status code is this number.The default value of code is true.If the second optional parameter close is true, close the Lua state machine before exiting.#2#a_1_0_0_1,b_1_0_0_1",
    "os.clock@os.clock ()\n  The approximate value of the second CPU time used by the return program.#0#",
    "os.date@os.date ([format [, time]])\n  Returns a string or table containing the date and time.The formatting method depends on the format of the given string.If the time parameter is provided, format the time (see the OS. Time function for this value).Otherwise, date formats the current time.If format is '!'heads, dates are formatted for coordinated universal time.After this optional character item, if the format is a string '* t,' the date return a list of the following fields: year (4 digits), month (1-12), day (1-31), hour (0-23), min (0-59), the SEC (0-61), wday (Saturday, Sunday for 1), yday (which day), and isdst (daylight saving time tag, a Boolean).For the last domain, if the information is not provided, it does not exist.If format is not '*t', date is returned as a string, and the formatting method follows the rules of the ISO C function strftime.If you do not call a parameter, date returns a reasonable date time string, depending on the host program and the current locale (that is, OS. Date () is equivalent to os.date('%c')).On a non-posix system, because this function relies on C functions gmtime and localtime, it may not be thread-safe.#2#format_1_1_1_,time_1_0_1_0",
    "os.difftime@os.difftime (t2, t1)\n  Returns the difference between t1 and t2 in seconds.The time here is the value returned by os-time.In POSIX, Windows, and some other systems, this value is t2 minus t1.#2#t1_1_0_1_0,t2_1_0_1_0",
    "os.getenv@os.getenv (varname)\n  Returns the value of the process environment variable varname, which returns nil if the variable is undefined.#1#varname_1_1_0_",
    "os.remove@os.remove (filename)\n  Deleting a file with the specified name (which can be an empty directory on the POSIX system) returns nil with an error description string and error code if the function fails.#1#filename_1_1_0_",
    "os.rename@os.rename (oldname, newname)\n  Rename a file or directory named oldname to newname.If the function fails, return nil with an error description string and error code.#2#oldname_1_1_0_,newname_1_1_0_",
    "os.setlocale@os.setlocale (locale [, category])\n  Sets the current area of the program.A locale is a system-related string for a locale;Category is an optional string that describes which category to change: 'all'', 'collate'', 'ctype'', 'monetary'', 'numeric'', or 'time'';The default category is 'all'.This function returns the name of the new region.If the request is not approved, return nil.When a locale is an empty string, the current locale is set to a local area defined in the implementation.When the locale is a string 'C', the current area is set to the standard C region.When the first parameter is nil, this function returns only the name of the category specified in the current locale.Since this function relies on the C function setlocale, it may not be thread-safe.#2#locale_1_1_0_,category_0_1_1_all_collate_ctype_monetary_numeric_time",
    "os.time@os.time ([table])\n  When the parameter is not passed, the current time is returned.If you pass in a table, you return the time represented by this table.This table must contain domain year, month, and day;You can include an hour (default is 12), min (0), SEC (0), and isdst (default is nil).For a detailed description of these domains, see the os-date function.The return value is a number that is determined by your system.In POSIX, Windows, and some other systems, this number counts the number of seconds that elapsed from the specified time ('epoch').For another system, the meaning is undefined, and you can only use the return number of time for the parameters of os-date and os-difftime.#1#table_1_0_1_",
    "os.tmpname@os.tmpname ()\n  Returns a filename string that can be used for temporary files.This file must be explicitly opened before use, and it needs to be explicitly deleted when it is no longer used.On the POSIX system, this function creates a file with the filename to avoid security risk.(other people may create this file at a time when they get the file name to create the file.)You still need to open it before you use it, and finally delete it (even if you don't use it).It is only possible that you should use IO. Tmpfile, since the file can be deleted automatically at the end of the program.#0#",///10

    "coroutine.create@coroutine.create (f)\n  Create a new co-path of the principal function f.F must be a function of Lua.Returns this new copath, which is an object of type 'thread'.#1#threadFunc_1_0_0_f",///11
    "coroutine.isyieldable@coroutine.isyieldable ()\n  Return true if the running copath is available.The current cocycle is available when it is not in the main thread or is not in a C function that cannot be released.#0#",
    "coroutine.resume@coroutine.resume (co [, val1, ···])\n  Start or continue the operation of co.When you first extend a co-program, it starts running from the main function.Val1,...These values are passed in as arguments to the body function.Resume will restart it if it is removed.Val1,...These parameters will act as the return value of the outgoing point.If the co-run does not fail, resume returns true plus all values that are passed to the yield (when the co-path is given), or all return values of the principal function (when the copath is aborted).If any error occurs, resume returns false plus error messages.#3#co_1_0_0_co,val1_1_0_1_val1,uncertain_1_0_2_...",
    "coroutine.running@coroutine.running ()\n  Returns a Boolean amount that is currently running.If the current run is the main thread, it is true.#0#",
    "coroutine.status@coroutine.status (co)\n  Returns the state of the cotrip co as a string: when it is running (it is the one calling status), it returns 'running'';Return 'suspended'' if the cotrip call yields or has not yet started to run.If the co-program is active, it is not running (that is, it is continuing other cooperations) and returns 'normal'';Returns 'dead'' if the coprogram runs out of the main function or stops due to an error.#1#co_1_0_0_co",
    "coroutine.wrap@coroutine.wrap (f)\n  Create a new co-path of the principal function f.F must be a function of Lua.Returns a function that will continue the process each time the function is called.The parameters passed to this function are all additional parameters for resume.It returns the same value as resume, except that there is no first Boolean.If any error occurs, throw this error.#1#threadFunc_1_0_0_f",
    "coroutine.yield@coroutine.yield (...)\n  Suspend the execution of the protocol being called.The parameters passed to yield will be converted to the additional return value of resume.#1#uncertain_1_0_2_...",///17

    "io.close@io.close ([file])\n  Equivalent to file:close().The default output file will be closed when no file is given.#1#file_1_0_1_",///18
    "io.flush@io.flush ()\n  Equivalent to IO. Output ():flush().#0#",
    "io.input@io.input ([file])\n  When you call it with a filename, open the file with the name (in text mode) and set the file handle to the default input file.If you call it with a file handle, simply set the handle to the default input file.If the call is not passed, it returns the current default input file.In the case of an error, the function throws the error instead of returning the error code.#1#file_1_0_1_",
    "io.lines@io.lines ([filename ...])\n  Opens the specified file name with the read mode and returns an iteration function.The working method of this iteration function is the same as using an open file to call file:lines(··).When the iteration function detects the end of the file, it does not return the value (let the loop end) and automatically close the file.The call IO. Lines () (no file name) is equivalent to IO. Input ():lines('*l');That is, it will enter the file according to the row iteration standard.In this case, it does not close the file after the loop ends.In the case of an error, the function throws the error instead of returning the error code.#2#filename_1_1_1_,uncertain_1_0_2_...",
    "io.open@io.open (filename [, mode])\n  This function opens a file with the mode specified by the string mode.Returns the new file handle.Returns nil plus error message when an error occurs.\nThe mode string can be any of the following values:\n'R ': read mode (default);\n'W ': write mode;\n'A ': additional mode;\n'R +': update mode, all previous data are reserved;\n'W +': update mode, delete all previous data;\n'A +': append update mode, all previous data is reserved, only allowed to be written at the end of the file.The mode string can add a 'b' at the end, which opens the file in binary mode on some systems.#2#filename_1_1_1_,mode_0_1_1_r_w_a_r+_w+_a+",
    "io.output@io.output ([file])\n  Similar to IO. Input.However, the default output file is used.#1#file_1_0_1_",
    "io.read@io.read (...)\n  Equivalent to IO. Input ():read(·).#1#uncertain_1_0_2_...",
    "io.tmpfile@io.tmpfile ()\n  Handle to return a temporary file.This file is opened in update mode and will be deleted automatically at the end of the program.#0#",
    "io.type@io.type (obj)\n  Check if obj is a valid file handle.If obj is an open file handle, return the string file'.If obj is a closed file handle, return the string 'closed file'.If obj is not a file handle, it returns nil.#1#obj_1_0_0_",
    "io.write@io.write (...)\n  Equivalent to IO. Output ():write(··).#1#uncertain_1_0_2_...",
    "file:close@file:close ()\n Close the file.Note that the file is automatically turned off when the handle is garbage collected, but the time is not expected.When you close the file handle created by ii.popen, file:close returns the same value returned by os.execute.#0#",
    "file:flush@file:flush ()\n  Save the written data to the file.#0#",
    "file:lines@file:lines (...)\n  Returns an iterator function that reads the data from the file in the specified format each time the iterator is called.If no format is specified, use the default value 'l'.Here's an exampleFor c in file:lines(1) do body end.The character will be read from the current position of the file.Unlike IO. Lines, this function does not close the file at the end of the loop.In the case of an error, the function throws the error instead of returning the error code.#1#uncertain_1_0_2_...",
    "file:read@file:read (...)\n  Read the file file, the specified format determines what to read.For each format, the function returns the string or number corresponding to the character read.If you cannot read the data in this format, you return nil.(for the last case, the function does not read the following format.)When the call is not formatted, it reads the next line in the default format (see below).The format provided is.\n'n ': reads a number, according to Lua's conversion grammar, may return floating-point number or integer.(Numbers can have front or back Spaces, and symbols.)As long as the number is legal, the format is always read as long as possible;If the prefixes that are read cannot form a valid number (such as an empty string, '0x'' or '3.4e-''), the function runs and returns nil.\n'i ': reads an integer and returns an integer value.\n'a ': read the entire file from the current location.If you are at the end of the file, return an empty string.\n'I ': read a line and ignore the end tag.When at the end of the file, return nil, which is the default format.\n'L ': read a line and leave the line end tag (if any), and return nil when the end of the file is returned.\nNumber: reads a string that does not exceed this number of bytes.When at the end of the file, return nil.If number is zero, it reads nothing and returns an empty string.When at the end of the file, return nil.The format 'l' and 'l' can only be used for text files.#1#uncertain_1_0_2_...",////01
    "file:seek@file:seek ([whence [, offset]])\n  Sets and retrieves the location computed at the beginning of the file.Set the location of the designated by the offset and whence string whence the basis of decision.The basis points can be:\n'Set ': the base point is 0 (the beginning of the file);\n'Cur ': the base point is the current position;\n'End ': the end of the document;When seek succeeds, returns the location of the file that is finally computed from the beginning of the file.When seek fails, returns nil plus an error description string.Whence the default value is 'cur', offset the default is 0.Therefore, call file:seek() returns the current location of the file and does not change it;Calling file:seek('set') sets the location to the beginning of the file (and returns 0);Calling file:seek('end') sets the location to the end of the file and returns the file size.#2#where_0_1_0_0_cur_set_end,offset _1_0_0_0",
    "file:setvbuf@file:setvbuf (mode [, size])\n  Sets the buffer mode for the output file.There are three modes:\n'No ': no buffer;The output operation takes effect immediately.\n'Full ': full buffer;You can only do the output if the cache is full or when you explicitly flush the file call flush (see IO. Flush).\n'Line ': line buffer;The output will be buffered to any input before each new line, for some special file (such as a terminal device).For the latter two cases, size specifies the buffer size in bytes.The default is to have an appropriate size.#2#mode_0_1_1_no_full_line,size _1_0_1_0",
    "file:write@file:write (...)\n  Write the value of the parameter to file one by one.The argument must be a string or a number.When successful, the function returns file.Otherwise return nil plus error description string.#1#uncertain_1_0_2_...",///34

    "math.abs@math.abs (x)\n  Return the absolute value of x.(integer/float)#1#x_1_0_0_0",////35
    "math.acos@math.acos (x)\n  Returns the inverse cosine of x in radians.#1#x_1_0_0_0",
    "math.asin@math.asin (x)\n  Returns the inverse sine of x in radians.#1#x_1_0_0_0",
    "math.atan@math.atan (y [, x])\n  Returns the tangent value of y/x in radians.It USES symbols of two parameters to find which quadrant the result falls in.(even if x is zero, you can handle it correctly.)The default x is 1, so calling math.atan(y) returns the inverse tangent of y.#2#y_1_0_0_0,x_1_0_1_1",
    "math.ceil@math.ceil (x)\n  Returns the minimum integer value not less than x.#1#x_1_0_0_0",
    "math.cos@math.cos (x)\n  Returns the cosine of x (assuming the parameter is radians).#1#x_1_0_0_0",
    "math.deg@math.deg (x)\n  I'm going to convert Angle x from radians to angles.#1#x_1_0_0_0",
    "math.exp@math.exp (x)\n  Returns the value of ex (e is the base of the natural log).#1#x_1_0_0_0",
    "math.floor@math.floor (x)\n  Returns the maximum integer value not greater than x.#1#x_1_0_0_0",
    "math.fmod@math.fmod (x, y)\n  Return x divided by y, the remainder of the quotient to zero.(integer/float).#1#x_1_0_0_0,x_1_0_0_0",
    "math.log@math.log (x [, base])\n  Returns the logarithm of x at the specified base.The default base is e (so this function returns the natural log of x).#2#x_1_0_0_0,base_1_0_1_e",
    "math.max@math.max (x, ...)\n  The maximum value in the return parameter is determined by the Lua operation.(integer/float).#2#x_1_0_0_0,uncertain_1_0_2_...",
    "math.min@math.min (x, ...)\n  The minimum value in the return parameter is determined by the Lua operation.(integer/float).#2#x_1_0_0_0,uncertain_1_0_2_...",
    "math.modf@math.modf (x)\n  Returns the integer part and the fractional part of x.The second result must be a float.#1#x_1_0_0_0",
    "math.rad@math.rad (x)\n  Convert Angle x from Angle to radian.#1#x_1_0_0_0",
    "math.random@math.random ([m [, n]])\n  When called without arguments, return a [0, 1] interval uniform distribution of floating-point pseudo-random number. When called with two integers m and n, math. The random returns a [m, n] interval uniform distribution of pseudo-random number of integers. (value m - n can't be negative, and must be within the scope of the Lua integer.) call math.h random (n) is equivalent to math. The random (1, n).This function is an encapsulation of the bit random number function provided by C.The statistical property is not guaranteed.#2#m_1_0_1_0,n_1_0_1_1",
    "math.randomseed@math.randomseed (x)\n  Set x to be the 'seed'' of the pseudo-random number generator: the same seed produces the same random sequence.#1#x_1_0_0_0",
    "math.sin@math.sin (x)\n  Returns the sine of x (assuming the parameter is radians).#1#x_1_0_0_0",
    "math.sqrt@math.sqrt (x)\n Return the square root of x.(you can also use exponentiation ^ 0.5 x to calculate this value.).#1#x_1_0_0_0",
    "math.tan@math.tan (x)\n  Returns the tangent of x (assuming the parameter is radians).#1#x_1_0_0_0",
    "math.tointeger@math.tointeger (x)\n  If x can be converted to an integer, return the integer.Otherwise, it returns nil.#1#x_1_0_0_0",
    "math.type@math.type (x)\n  If x is an integer, return 'integer'', if it's a float, return 'float'', if x is not a number, return nil.#1#x_1_0_0_0",
    "math.ult@math.ult (m, n)\n  If the integer m and n are compared in unsigned integer form, m is below n, return Boolean true or return false.#1#m_1_0_0_0,n_1_0_0_0",///57

    "string.byte@string.byte (s [, i [, j]])\n  If the integer m and n are compared in unsigned integer form, m is below n, return Boolean true or return false., the internal digital code of s[j].The default value of I is 1;The default value of j is I.These indexes are modified with the rule of function string. Sub.#3#s_1_1_0_,i_1_0_1_1,j_1_0_1_1",////58
    "string.char@string.char (...)\n  If the integer m and n are compared in unsigned integer form, m is below n, return Boolean true or return false.Returns a string of the same length as the parameter.The internal encoding value of each character is equal to the corresponding parameter value.Digital coding does not have to be cross-platform.#1#uncertain_1_0_2_...",
    "string.dump@string.dump (function [, strip])\n  If the integer m and n are compared in unsigned integer form, m is below n, return Boolean true or return false.You can then use load to call the string to get a copy of the function (but to bind the new value).If the strip is true, the binary code block does not carry the debug information for the function (local variable name, line number, etc.).A function with a value only holds the number of values.When (again) loading, these upper values are updated to an instance of nil.(you can use the debug library to serialize the values in the way you need them, and reload them into the function).#2#function_1_0_1_func,strip_1_0_1_0",
    "string.find@string.find (s, pattern [, init [, plain]])\n  If the integer m and n are compared in unsigned integer form, m is below n, return Boolean true or return false.If a match is found, find returns an index of its starting and ending position in s;Otherwise, it returns nil.The third optional numeric argument init indicates where to start the search;The default value is 1 and can be negative.When the fourth optional parameter plain is true, close the pattern matching mechanism.At this point, the function only does the direct 'find substring'' operation, while no characters in pattern are considered as magic characters.Note that if a plain is given, you must write init.If the capture is defined in the schema, a number of values captured will also be returned after two indexes.#4#s_1_1_0_,pattern_1_1_0_,init_1_0_1_1,plain_1_0_1_0",
    "string.format@string.format (formatstring, ...)\n  If the integer m and n are compared in unsigned integer form, m is below n, return Boolean true or return false.The formatting string follows the rules of the ISO C function sprintf.The difference is that choice *, h, L, L, n, p doesn't support, plus an option q.The q option formats a string as two double quotes, and does the proper escape string for the inner character.This string can be safely read by the Lua interpreter.#2#formatstring_1_1_0_,uncertain_1_0_2_...",
    "string.gmatch@string.gmatch (s, pattern)\n  If the integer m and n are compared in unsigned integer form, m is below n, return Boolean true or return false.Each call to this function will continue to the pattern (see § 6.4.1) match for s, and returns the value of all captured.If no capture is specified in the pattern, the entire pattern is captured each time.#2#s_1_1_0_,pattern_1_1_0_",
    "string.gsub@string.gsub (s, pattern, repl [, n])\n  If the integer m and n are compared in unsigned integer form, m is below n, return Boolean true or return false.A repl can be a string, a table, or a function.The gsub also returns a total number of matches in the second return value.The name gsub comes from Global SUBstitution.If repl is a string, then use this string as a replacement.The character % is an escape character: all of the forms in repl represent a string of %d, and d can be 1 through 9.String %0 represents the entire match.A string %% represents a single %.If the repl is a table, the first catch is used as the key to check the table.If repl is a function, this function is called every time a match occurs.All captured substrings are passed in as parameters in turn.In any case, no capture in the template is seen as capturing the entire template.If a table's query result or function returns a string or a number, it is used as a substitution string;Instead of substituting (that is, the original string before the match is retained) when false or nil is returned.#4#s_1_1_0_,pattern_1_1_0_,repl_1_0_0_,n_1_0_1_0",
    "string.len@string.len (s)\n  If the integer m and n are compared in unsigned integer form, m is below n, return Boolean true or return false.The length of the empty string is 0.The embedded zeros are also counted, so the length of 'a\000bc\000' is 5.#1#s_1_1_0_",
    "string.lower@string.lower (s)\n  If the integer m and n are compared in unsigned integer form, m is below n, return Boolean true or return false.The other strings will not be changed.The definition of the uppercase character depends on the current locale.#1#s_1_1_0_",
    "string.match@string.match (s, pattern [, init])\n  If the integer m and n are compared in unsigned integer form, m is below n, return Boolean true or return false.If you can find it, match returns the catch;Otherwise, it returns nil.If the capture is not specified in the pattern, returns the string captured by the entire pattern.The third optional numeric argument init indicates where to start the search;It defaults to 1 and can be negative.#3#s_1_1_0_,pattern_1_1_0_,init_1_0_1_1",
    "string.pack@string.pack (fmt, v1, v2, ...)\n  If the integer m and n are compared in unsigned integer form, m is below n, return Boolean true or return false.String FMT for packaging format (see § 6.4.2).#4#fmt_1_1_0_,v1_1_0_0_v1,v2_1_0_0_v2,uncertain_1_0_2_...",
    "string.packsize@string.packsize (fmt)\n  If the integer m and n are compared in unsigned integer form, m is below n, return Boolean true or return false.In the format string can't change long option 's' or 'z' (see § 6.4.2).#1#fmt_1_1_0_",
    "string.rep@string.rep (s, n [, sep])\n  If the integer m and n are compared in unsigned integer form, m is below n, return Boolean true or return false.The default sep value is an empty string (i.e., no separator).Returns an empty string if n is not positive.#3#s_1_1_0_,n_1_0_0_1,sep_1_1_1_",
    "string.reverse@string.Returns the reverse string of string s.#1#s_1_1_0_",
    "string.sub@string.sub (s, i [, j])\n  Returns the substring of s, which starts from I to j;Both I and j can be negative.If you don't give j, it's minus 1 (the same length as the string).In particular, the call string. Sub (s,1,j) can return the prefix string of s with the length of j, while string. Sub (s, -i) returns the suffix string of length I.If I is less than 1 after the negative index is escaped, I will fix it back to 1.If the j is larger than the length of the string, it is fixed as a string length.If I is greater than j after correction, the function returns an empty string.#3#s_1_1_0_,i_1_0_0_0,j_1_0_0_0",
    "string.unpack@string.unpack (fmt, s [, pos])\n  Return to format FMT (see § 6.4.2) packaging in the string s (see string. Pack) values.Option pos (default 1) marks where to start from s.After reading all the values, the function returns the first unread byte in s.#3#fmt_1_1_0_,s_1_1_0_,pos_1_0_1_1",
    "string.upper@string.upper (s)\n  Takes a string, converts all lowercase characters to uppercase and returns a copy.The other strings will not be changed.The definition of lowercase characters depends on the current locale.#1#s_1_1_0_",

    "timerRun@timerRun(id),Start the system timer with the specified id\n    Parameter 1 id: system timer id number, 0-35\n  Return value: result of operation 0 - failure, 1 success#1#id_1_0_0_0",
    "timerStop@timerStop(id),Stop the system timer with the specified id\n    Parameter 1 id: system timer id number, 0-35\n  Return value: result of operation 0 - failure, 1 success#1#id_1_0_0_0",
    "timerReset@timerReset(id,value),Set the current value of the timer, set by the second parameter\n   Parameter 1 id: system timer id number, 0-35\n   Parameter 2 value: set value\n   Return value: operation result 0 - failed, 1 - successful#2#id_1_0_0_0,value_1_0_0_0",
    "timerSkip@timerSkip(id,value),Add/subtract the specified value on the current time of the specified timer\n   Parameter 1 id: system timer id number, 0-35\n   Parameter 2 value: the specified value of addition and subtraction\n   Return value: operation result 0 - failed, 1 - successful#2#id_1_0_0_0,value_1_0_0_0",
    "timerValue@timerValue(id),Take the current value of the specified timer\n   Parameter 1 id: system timer id number, 0-35\n   Return value: the data retrieved#1#id_1_0_0_0",
    "timerStr@timerStr(id,type),Returns the current value of the specified timer as a string\n   Parameter 1 id: system timer id number, 0-35\n   Parameter 2 type: string format\n   Return value: the string retrieved#2#id_1_0_0_0,type_1_0_0_0",
    "timerState@timerState(id),Take the working state of the specified timer\n   Parameter 1 id: system timer id number, 0-35\n   Return value: working status, 0-timer stop, 1-timer running#1#id_1_0_0_0",
    "timerSetLimit@timerSetLimit(id,num,type),Set the maximum value of the specified timer, that is, set the upper limit of the timer.\n   Parameter 1 id: system timer id number, 0-35\n   Parameter 2 num: maximum value, integer data\n   Parameter 3 type: the action type when the timer reaches the upper limit, 1 - means stop after running to the upper limit; 0 - means run after re-circulation after the upper limit\n   Return value: operation result 0 - failed, 1 - successful#3#id_1_0_0_0,value_1_0_0_0,type_1_0_0_0",
    "timerSetOutput@timerSetOutput(id,linkid,reg,index,station),Set the value output address of the specified timer\n   Parameter 1 id: system timer id number, 0-35\n   Parameter 2 link: the id or connection name of the connection\n   Parameter 3 reg: register name\n   Parameter 4 index : register index\n   Parameter 5 station : equipment station number\n   Return value: operation result 0 - failed, 1 - successful#5#id_1_0_0_0,linkid_0_0_0_0,reg_0_1_0_0,index_1_0_0_0,station_1_0_0_0",
    "timerWaitFor@timerWaitFor(value),Wait until the specified time to execute down\n   Parameter 1 value: the specified value, integer data, ms\n   Return value: none#1#value_1_0_0_0",
    "timerOutRun@timerOutRun(id,id),Run the specified script when the specified timer reaches the upper limit\n   Parameter 1 id: system timer id number, 0-35\n   Parameter 2 id: the specified script id\n   Return value: operation result 0 - failed, 1 - successful#2#id_1_0_0_0,id_1_0_0_0",

    "initCommunication@initCommunication(type,par),Initialize the communication interface\n   Parameter 1 type: 0-serial port, 1-network port\n   Parameter 2 par: communication parameter, string type\n              For serial port, for example, 1-9600-8-1-NONE, the first segment com number, the second segment baud rate, the third segment data bit, the fourth segment stop bit, the fifth segment check\n              For the network port, for example, 1-0-192.168.1.100-8000, the first segment 0-udp1-tcp, the second segment 0-client 1-server, the third segment represents the ip address, and the fourth segment represents the port.\n   Return value: Communication id#2#type_1_0_0_0,par_1_1_0_1-9600-8-1-NONE",
    "sendData@sendData(),Send data via communication id\nFirst form\n   Parameter 1 id: communication id\n   Parameter 2 s: written string\n   Parameter 3 len: the length of the string to be written\n   Parameter 4 timeout: write timeout\n   Parameter 5 frameType: frame string format, asc, hex\n   Parameter 6 clientIdOrUdpPort: When the communication is created as the tcp server, it is the connected client id, and the udp communication is the remote port number.\n   Parameter 7 udpIp:The communication is Udp is the remote ip address, otherwise this parameter is meaningless.\n   Return value: the length of the actual transmission, communication error when small and 0\nSecond form\n   Parameter 1 id: communication id\n   Parameter 2 array: byte array written\n   Parameter 3 len: length of byte array written\n   Parameter 4 timeout: write timeout\n   Parameter 5 clientIdOrUdpPort: When the communication is created as the tcp server, it is the connected client id, and the udp communication is the remote port number.\n   Parameter 6 udpIp:The communication is Udp is the remote ip address, otherwise this parameter is meaningless.\n   Return value: the length of the actual transmission, communication error when small and 0#7#id_1_0_0_0,s_1_1_0_,len_1_0_0_0,delaytime(ms)_1_0_0_200,frameType_1_1_1_asc,clientId_1_0_1_0,udpIp_1_1_1_0.0.0.0",
    "recvData@recvData,Read data by communication id\nFirst form\n   Parameter 1 id: communication id\n   Parameter 3 len: read length\n   Parameter 4 timeout: read timeout\n   Parameter 5 clientId: The client id that was connected when the communication was created as the tcp server.\n   Return value: the format of the received data string is returned\nSecond form\n   Parameter 1 id: communication id\n   Parameter 2 array: the byte array read\n   Parameter 3 len: read length\n   Parameter 4 timeout: read timeout\n   Parameter 5 clientId: The client id that was connected when the communication was created as the tcp server.\n   Return value: the length of the actual read, communication error when small and 0#4#id_1_0_0_0,len_1_0_0_0,delaytime(ms)_1_0_0_200,clientId_1_0_1_0",
    "getLinkNum@getLinkNum,The tcp server obtains the number of connected clients.\n   Parameter 1 id: communication id\n   Return value: the number of connected clients#1#id_1_0_0_0",
    "closeCommunication@closeCommunication,Turn off the communication function of the specified id.\n   Parameter 1 id: communication id\n   Return value: none#1#id_1_0_0_0"
};

////4.常用语句结构
const int MACROFUNCTYPE4NUM = 5;
const QString MACROFUNCTYPE4NAME_CN[] =
{
    QString::fromUtf8("if语句@if(布尔表达式)\nthen\n   --[ 在布尔表达式为 true 时执行的语句 --]\nend#0#"),
    QString::fromUtf8("if...else语句@if(布尔表达式)\nthen\n   --[ 布尔表达式为 true 时执行该语句块 --]\nelse\n   --[ 布尔表达式为 false 时执行该语句块 --]\nend#0#"),
    QString::fromUtf8("while循环@while(条件)\ndo\n   执行语句\nend#0#"),
    QString::fromUtf8("for循环@数值for循环\n  for var=exp1,exp2,exp3 do  \n      <执行体>  \n  end\n  var从exp1变化到exp2，每次变化以exp3为步长递增var，并执行一次\"执行体\"。exp3是可选的，如果不指定，默认为1。\n泛型for循环\n  泛型for循环通过一个迭代器函数来遍历所有值\n  --打印数组a的所有值  \n  for i,v in pairs(a) \n      do print(v) \n  end \n  i是数组索引值，v是对应索引的数组元素值。pairs是Lua提供的一个迭代器函数，用来迭代数组。\n  实例:\n  循环数组 days：\n  days = {\"Sunday\",\"Monday\",\"Tuesday\",\"Wednesday\",\"Thursday\",\"Friday\",\"Saturday\"}  \n  for i,v in pairs(days) \n      do  print(v) \n  end   \n  以上实例输出结果为：\n  Sunday\n  Monday\n  Tuesday\n  Wednesday\n  Thursday\n  Friday\n  Saturday#0#"),
    QString::fromUtf8("repeat...until循环@repeat\n   执行语句\nuntil( 条件 )\n我们注意到循环条件判断语句在循环体末尾部分，所以在条件进行判断前循环体都会执行一次。\n如果条件判断语句为 false，循环会重新开始执行，直到条件判断语句为 true 才会停止执行。#0#"),
};
const QString MACROFUNCTYPE4NAME_EN[] =
{
    "if statement@if (Boolean expression)\nthen\n    --[ statement executed when boolean expression is true --]\nend#0#",
    "if...else statement@if (Boolean expression)\nthen\n    --[ Execute the block when the boolean expression is true --]\nelse\n    --[ Execute the block when the boolean expression is false --]\nend#0#",
    "while loop@while(condition)\ndo\n   statements\nend#0#",
    "for loop@Numerical for loop\n   for var=exp1,exp2,exp3 do\n       <execution body>\n   end\n   Var changes from exp1 to exp2, incrementing var in increments of exp3 for each change, and executing an \"execution\". Exp3 is optional. If not specified, the default is 1.\nGeneric for loop\n   The generic for loop loops through all the values through an iterator function\n   -- print all values of array a\n   for i,v in pairs(a)\n       do print(v)\n   end\n   i is the array index value, and v is the array element value of the corresponding index. pairs is an iterator function provided by Lua to iterate over arrays.\n   Example:\n   Loop array days:\n   Days = {\"Sunday\", \"Monday\", \"Tuesday\", \"Wednesday\", \"Thursday\", \"Friday\", \"Saturday\"}\n   for i,v in pairs(days)\n       do print(v)\n   end\n   The output of the above example is:\n   Sunday\n   Monday\n   Tuesday\n   Wednesday\n   Thursday\n   Friday\n   Saturday#0#",
    "repeat...until loop@repeat\n    Statements\nuntil( condition )\nWe noticed that the condition of the loop condition is at the end of the loop body, so the loop body will execute once before the condition is judged.\nIf the conditional statement (condition) is false, the loop will restart execution until the conditional statement (condition) is true.#0#",
};
const QString MACROFUNCTYPE4NAME_EDIT[] =
{
    QString::fromUtf8("if语句"),
    QString::fromUtf8("if...else语句"),
    QString::fromUtf8("while循环"),
    QString::fromUtf8("for循环"),
    QString::fromUtf8("repeat...until循环"),
    "if statement",
    "if...else statement",
    "while loop",
    "for loop",
    "repeat...until loop"
};
const QString MACROFUNCTYPE4NAME_EDIT2[] =
{
    "if()\nthen\n   \nend",
    "if()\nthen\n   \nelse\n   \nend",
    "while()\ndo\n   \nend",
    "for  do\n     \nend",
    "repeat\n   \nuntil()"
};
////输入自动补全提示
const int MACROINPUTAUTOFILLEDNUM= 143;
const QString MACROINPUTAUTOFILLEDNAME[] =
{
    "setdata(\"Link1\",\"LW\",index,station,len,value,valueType)",
    "getdata(\"Link1\",\"LW\",index,station,len,valueType,type)",
    "setdata(\"Link1\",\"LW\",index,station,len,array,valueType)",
    "getdata(\"Link1\",\"LW\",index,station,len,valueType,array,type)",
    "getHmiModel()",
    "suspendlink(\"linkname\")",
    "startlink(\"linkname\")",
    "switchScreen(screenID)",
    "function",
    "checkMethod()",
    "getdevicemutex(linkid)",
    "releasedevicemutex(linkid)",
    "HIBYTE(num)",
    "LOBYTE(num)",
    "local",
    "writedevice()",
    "readdevice()",
    "getdevicemutex(id)",
    "releasedevicemutex(id)",
    "hmiPrint()",
    "logdevice()",
    "setSystemParameter(path,value,valueType,saveFlag)",
    "getSystemParameter(path,valueType)",
    "runScriptFromId(id)",
    "runScriptText(text)",
    "mouseClick(x,y)",
    "mouseClickAbs(x,y)",

    "coroutine.create(f)",
    "coroutine.isyieldable()",
    "coroutine.resume(co [, val1, ...])",
    "coroutine.running()",
    "coroutine.status(co)",
    "coroutine.wrap(f)",
    "coroutine.yield(...)",

    "io.close([file])",
    "io.flush()",
    "io.lines([filename ...])",
    "io.open(filename [, mode])",
    "io.output([file])",
    "io.input([file])",
    "io.read(...)",
    "io.tmpfile()",
    "io.type(obj)",
    "io.write(...)",
    "file:close()",
    "file:flush()",
    "file:lines(...)",
    "file:read(...)",
    "file:seek([whence [, offset]])",
    "file:setvbuf(mode [, size])",
    "file:write(...)",

   "math.abs(x)",
    "math.acos(x)",
    "math.asin(x)",
    "math.atan(y [, x])",
    "math.ceil(x)",
    "math.cos(x)",
    "math.deg(x)",
    "math.exp(x)",
    "math.floor(x)",
    "math.fmod(x, y)",
    "math.huge",
    "math.log(x [, base])",
    "math.max(x,...)",
    "math.maxinteger",
    "math.min(x,...)",
    "math.mininteger",
    "math.modf(x)",
    "math.pi",
    "math.rad(x)",
    "math.random([m [, n]])",
    "math.randomseed(x)",
    "math.sin(x)",
    "math.sqrt(x)",
    "math.tan(x)",
    "math.tointeger(x)",
    "math.type(x)",
    "math.ult(m, n)",

    "os.clock()",
    "os.date([format [,time]])",
    "os.difftime(t2, t1)",
    "os.execute([command])",
    "os.exit([code [,close]])",
    "os.getenv(varname)",
    "os.remove(filename)",
    "os.rename(oldname, newname)",
    "os.setlocale(locale [,category])",
    "os.time([table])",
    "os.tmpname()",

   "string.byte(s [,i [,j]])",
    "string.char(...)",
    "string.dump(function [,strip])",
    "string.find(s, pattern [,init [, plain]])",
    "string.format(formatstring,...)",
    "string.gmatch(s, pattern)",
    "string.gsub(s, pattern, repl [, n])",
    "string.len(s)",
    "string.lower(s)",
    "string.match(s, pattern [, init])",
    "string.pack(fmt, v1, v2,...)",
    "string.packsize(fmt)",
    "string.rep(s, n [, sep])",
    "string.reverse(s)",
    "string.sub(s, i [, j])",
    "string.unpack(fmt, s [, pos])",
    "string.upper(s)",

    "utf8.char(...)" ,
    "utf8.charpattern",
    "utf8.codes(s)",
    "utf8.codepoint(s [, i [, j]])",
    "utf8.len(s [, i [, j]])",
    "utf8.offset(s, n [, i])",

    "DATA_TYPE_BIT",
    "DATA_TYPE_BB",
    "DATA_TYPE_BUB",
    "DATA_TYPE_BD",
    "DATA_TYPE_WB",
    "DATA_TYPE_WUB",
    "DATA_TYPE_WD",
    "DATA_TYPE_DB",
    "DATA_TYPE_DUB",
    "DATA_TYPE_DD",
    "DATA_TYPE_DF",
    "DATA_TYPE_STR_UNI",
    "DATA_TYPE_STR_ASC",
    "CYCLE",
    "TRIGGER",

    "timerRun(id)",
    "timerStop(id)",
    "timerReset(id,value)",
    "timerSkip(id,value)",
    "timerValue(id)",
    "timerStr(id,type)",
    "timerState(id)",
    "timerSetLimit(id,num,type)",
    "timerSetOutput(id,linkid,reg,index,station)",
    "timerWaitFor(value)",
    "timerOutRun(id,id)",

    "initCommunication(type,par)",
    "sendData(id,data,len,timeout,type)",
    "recvData(id,len,timeout)" ,
    "getLinkNum(id)",
    "closeCommunication(id)"
};

const int MACROFUNCCHECKNUM= 1;
const QString MACROFUNCCHECKNAME[] =
{
    "print(s)"
};

////鼠标长停留提示消息
const int MACROFUNCTIPNUM= 8;
const QString MACROFUNCTIPNAME[] =
{
    "\\bsetdata\\b@setdata(string strlink,string reg,int index,int station,int len,int data)\nset internal and external address value",
    "\\bgetdata\\b@getdata(string strlink,string reg,int index,int station,int len)\nget internal and external address value",
    "\\bgetHmiModel\\b@getHmiModel()\nObtain the device model",
    "\\bexecute\\b@os.execute(command)\nsystem function",
    "\\bexit\\b@os.exit(1,1)\nexit program",

    "\\bsuspendlink\\b@suspendlink(string strlink)\nsuspend the link by name",
    "\\bstartlink\\b@startlink(string strlink)\nstart the link by name",
     "\\bswitchScreen\\b@switchScreen(int id ,string name)\nswitch the screen by index and name"
};

/*【功能优化】_脚本功能添加命令到下拉框中_yangjindong_20180917_Begin*/
const int MACROSYSTEMPARNUM = 50;
const QString  MACROSYSTEMPAR[]=
{
    "link-Link1-linkName",
    "link-Link1-linkType",
    "link-Link1-linkDevice",
    "link-Link1-linkDriver",
    "link-Link1-HMIPosition",
    "link-Link1-HMIIP",
    "link-Link1-HMIPort",
    "link-Link1-BaudRate",
    "link-Link1-DataBit",
    "link-Link1-Check",
    "link-Link1-StopBit",
    "link-Link1-ScreenID",
    "link-Link1-PLCID",
    "link-Link1-CommTime",
    "link-Link1-Timeout1",
    "link-Link1-Timeout2",
    "link-Link1-RetryTimes",
    "link-Link1-AddressMode",
    "link-Link1-AddressStep",
    "HMI Sys Setting-Screen Model",
    "userExit",
    "HMI Sys Setting-Splash Screen Index",
    "HMI Sys Setting-Guard Times",
    "HMI Sys Setting-Touch Voice",
    "HMI Sys Setting-Screen Protection Use",
    "HMI Sys Setting-Screen Protection Index",
    "HMI Sys Setting-Load Logo Screen Use",
    "HMI Sys Setting- Load Logo Screen Index",
    "HMI Sys Setting-alarm voice",
    "HMI Sys Setting-alarm blink",
    "HMI Sys Setting-alarm bg color",
    "HMI Sys Setting-Flicker Frequency",
    "HMI Sys Setting-System Alarm-Use System Alarm",
    "HMI Sys Setting-System Alarm-Alarm background",
    "HMI Sys Setting-System Alarm-Alarm text color",
    "HMI Sys Setting-System Alarm-Family",
    "HMI Sys Setting-System Alarm-Font size",
    "HMI Sys Setting-System Alarm-Display mode",
    "HMI Sys Setting-System Alarm-Display position",
    "HMI Sys Setting-cursor hide",
    "HMI Sys Setting-font-font_family",
    "HMI Sys Setting-font-font_pixelSize",
    "HMI Sys Setting-font-font_color",
    "HMI Sys Setting-font-font_letterSpacing",
    "HMI Sys Setting-font-text_position",
    "HMI Sys Setting-font-font_bold",
    "HMI Sys Setting-font-font_underline",
    "HMI Sys Setting-font-font_italic",
    "pictureZoom",
    "HMI Sys Setting-downUpdateRecipe",
};
QString  MACROSYSTEMPAR_EN[]=
{
    "\npath Parameter Description:link-Link1-linkName\nType:string",
    "\npath Parameter Description:link-Link1-linkType\nType:string",
    "\npath Parameter Description:link-Link1-linkDevice\nType:string",
    "\npath Parameter Description:link-Link1-linkDriver\nType:string",
    "\npath Parameter Description:link-Link1-HMIPosition\nType:int",
    "\npath Parameter Description:link-Link1-HMIIP\nType:string",
    "\npath Parameter Description:link-Link1-HMIPort\nType:string",
    "\npath Parameter Description:link-Link1-BaudRate\nType:string\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:link-Link1-DataBit\nType:string\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:link-Link1-Check\nType:string\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:link-Link1-StopBit\nType:string\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:link-Link1-ScreenID\nType:string",
    "\npath Parameter Description:link-Link1-PLCID\nType:string",
    "\npath Parameter Description:link-Link1-CommTime\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:link-Link1-Timeout1\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:link-Link1-Timeout2\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:link-Link1-RetryTimes\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:link-Link1-AddressMode\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:link-Link1-AddressStep\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-Screen Model\nType:string",
    "\npath Parameter Description:userExit\nType:int",
    "\npath Parameter Description:HMI Sys Setting-Splash Screen Index\nType:int\nremark:This function needs to be restarted to take effect",
     "\npath Parameter Description:HMI Sys Setting-Guard Times\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-Touch Voice\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-Screen Protection Use\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-Screen Protection Index\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-Load Logo Screen Use\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-Load Logo Screen Index\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-alarm voice\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-alarm blink\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-alarm bg color\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-Flicker Frequency\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-System Alarm-Use System Alarm\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-System Alarm-Alarm background\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-System Alarm-Alarm text color\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-System Alarm-Family\nType:string\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-System Alarm-Font size\nType:in\nremark:This function needs to be restarted to take effectt",
    "\npath Parameter Description:HMI Sys Setting-System Alarm-Display mode\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-System Alarm-Display position\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-cursor hide\nType:int\nremark:This function needs to be restarted to take effect\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-font-font_family\nType:string\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-font-font_pixelSize\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-font-font_color\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-font-font_letterSpacing\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-font-text_position\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-font-font_bold\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-font-font_underline\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-font-font_italic\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:pictureZoom\nType:int\nremark:This function needs to be restarted to take effect",
    "\npath Parameter Description:HMI Sys Setting-downUpdateRecipe\nType:int",
};
QString  MACROSYSTEMPAR_CN[]=
{
    QString::fromUtf8("\npath参数说明：通讯-连接名称\n类型：string"),
    QString::fromUtf8("\npath参数说明：通讯-连接接口\n类型：string"),
    QString::fromUtf8("\npath参数说明：通讯-连接服务\n类型：string"),
    QString::fromUtf8("\npath参数说明：通讯-连接设备\n类型：string"),
    QString::fromUtf8("\npath参数说明：HMI位置\n类型：int"),
    QString::fromUtf8("\npath参数说明：通讯-远程ip\n类型：string"),
    QString::fromUtf8("\npath参数说明：通讯-远程端口号\n类型：string"),
    QString::fromUtf8("\npath参数说明：通讯参数-波特率\n类型：string\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：通讯参数-数据位\n类型：string\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：通讯参数-校验位\n类型：string\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：通讯参数-停止位\n类型：string\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：触摸屏站号\n类型：string"),
    QString::fromUtf8("\npath参数说明：PLC站号\n类型：string"),
    QString::fromUtf8("\npath参数说明：通信时间\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：超时时间1\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：超时时间2\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：重试次数\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：地址模式\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：PLC连续时间地址间隔\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：屏型号\n类型：string"),
    QString::fromUtf8("\npath参数说明：用户退出时间\n类型：int"),
    QString::fromUtf8("\npath参数说明：启动画面\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：启动-屏保时间\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：启动-触摸声音\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：打开屏保画面\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：设置屏保画面\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：打开启动-载入LOGO画面\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：设置启动-载入LOGO画面\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：报警声音\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：报警闪烁画面\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：报警-背景色\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：报警-闪烁频率\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：使用系统报警\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：未报警时背景色\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：报警-文本颜色\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：报警-字体\n类型：string\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：报警-字体大小\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：报警显示方式\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：报警位置\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：隐藏鼠标\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：字体\n类型：string\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：字体大小\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：字体颜色\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：字间隔\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：字体-位置\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：字体-加粗\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：字体-下划线\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：字体-斜体\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：图片缩小倍数\n类型：int\n备注：此功能需要重启才能生效"),
    QString::fromUtf8("\npath参数说明：下载配方\n类型：int"),
};
/*【功能优化】_脚本功能添加命令到下拉框中_yangjindong_20180917_End*/
#endif // MACROCONFIG

