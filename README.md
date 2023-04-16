# sms-forward
sms forwarding with Air700e

只在Air700e(移动卡)上测试过，Air780E应该也可以。

通过SMTP转发短信至指定邮箱，需要SIM卡有流量。

## 配置

修改 sms_forward_config.h中的邮箱配置

发送邮箱只测试过163邮箱，其他邮箱可能有所不同。

## 编译

需要CSDK
https://gitee.com/openLuat/luatos-soc-2022

按照其Readme将sms_forward文件夹添加到project目录下。

## 运行

上电按Power运行。