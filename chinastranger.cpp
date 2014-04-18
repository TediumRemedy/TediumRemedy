#include "chinastranger.h"
#include "chinastranger.h"


//moluren.net/chat protocol

/*

request:
GET http://moluren.net/json/connect?_token_id_=DF95957EA49AFC601E17D5385D15753D5834B8BB44C694B57B345CDEEEA9C95391EEC2BD168FB9BF&t=1392693914634

response:
{"connect":false}

request:
http://moluren.net/json/distance?_token_id_=DF95957EA49AFC601E17D5385D15753D5834B8BB44C694B57B345CDEEEA9C95391EEC2BD168FB9BF&t=1392693916603

response:
{"distance":0}

request:
http://moluren.net/json/disconnect?_token_id_=DF95957EA49AFC601E17D5385D15753D5834B8BB44C694B57B345CDEEEA9C95391EEC2BD168FB9BF&t=1392693943680

response:
{"r":"5302d34484aedf42945e28d2","disconnect":true}

request:
http://you.moluren.net/json/message/count?_callback=jQuery1830018213209146165643_1392693891917&_=1392694508434

response:
jQuery1830018213209146165643_1392693891917({"c":0})

request:
http://you.moluren.net/json/message/count?_callback=jQuery1830018213209146165643_1392693891917&_=1392694515435

response:
jQuery1830018213209146165643_1392693891917({"c":0})




other:
ip.src==64.237.33.20 || ip.dst==64.237.33.20


..{"s":{"c":580,"a":1158}}.~..{"hb":{"conn":true,"msgs":[],"s":{"s":"connected","c":"web"},"r":null,"pol":false,"p":{"s":"connect","c":"web"},"pt":false,"h":0}}..{"s":{"c":576,"a":1152}}..{"s":{"c":576,"a":1154}}...4.......E...k...]...I....Xt~"..`qzN'_5.b_+.kdzX'_..vgzN'^.*',z+Zh9.al=V?"iG<2nM69kD58mVx.9{"msg_sent":"msg_sent","say":"1392693930085","msg":"^_^"}..{"s":{"c":576,"a":1157}}..^l..%NC.;.>.|3q.93o.0.>.|3C.-.>.|.u.6
s.rNC.6
r.2.>.|]/.lZ%.g_..l]>..<{"msg_sent":"msg_sent","say":"1392693932421","msg":"ni hao"}..{"s":{"c":576,"a":1158}}..{"s":{"c":578,"a":1161}}.y{"hb":{"conn":false,"msgs":[],"s":{"s":"disconnect","c":"web"},"r":null,"pol":true,"p":{"s":"","c":""},"pt":false,"h":0}}

*/


ChinaStranger::ChinaStranger(QObject *parent) :
    CometClient(parent)
{
}
