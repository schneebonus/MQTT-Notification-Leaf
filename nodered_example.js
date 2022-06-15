p = 0;

global.get("humidity") > 60? color = "BLUE" : color = "BLACK";

segment = {
    panel: p,                   // panel index
    mode: "FX_MODE_STATIC",     // led mode
    color: color,               // led color
    speed: 1000,                // mode speed
    reverse: false,             // reverse mode
}

msg.payload = segment;
msg.topic = "notification/leaf";
return msg;
