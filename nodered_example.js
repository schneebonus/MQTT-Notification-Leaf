/*
Set first led to red breathing.
*/
id = 0;

segment = {
    index: id,                   // segment index
    start: id,                   // first led
    stop:  id,                   // last led
    mode: "FX_MODE_BREATH",     // led mode
    color: "RED",               // led color
    speed: 1000,                // mode speed
    reverse: false,             // reverse mode
}

msg.payload = segment;
msg.topic = "notification/pattern";
return msg;
