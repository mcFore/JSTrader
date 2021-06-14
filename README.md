# DataRecorder
CTP data record ,open with VS2013.

database:MongoDB

- 上期所的tradingday和其他交易所的tradingday就不一样，所以这里需要处理一下: 把date换成服务器本地的时间，time还可以用他给你传过来的
- 每天凌晨时候交易所可能发一些垃圾数据过来:
-     情况1 在早上9点开盘 推了一个数据包 数据包的时间戳写着早上5:00:00 ，这个肯定是会存进去 是一个BUG数据
-     情况2 在 凌晨3点推了一个 数据包 数据包上的时间戳写着早上9:20:00 时间戳是对的，但是凌晨3点压根没交易，所以也会存进去产生BUG
-     还有 若volume为0的tick, 其实也需要过滤一下, 周六日节假日也需要过滤. 如果你有办法，没办法的话就在周六日和节假日关闭数据接收
- https://zhuanlan.zhihu.com/p/24736528
