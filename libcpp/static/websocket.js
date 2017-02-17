
function htmltag(addr, onsetupdate) {
    this.websocket = undefined;
    this.addr = addr;
    this.onsetupdate = onsetupdate;
}

htmltag.prototype.start = function() {
    var ja = this;
    
    this.websocket = new WebSocket(this.addr); // ,'base64'
    this.isopen = 0;
    this.isclose = 0;
    
    this.websocket.onopen = function(evt) {
	console.log("[+] Websocket opened" + this.addr);
	ja.isopen = 1;
	ja.websocket.send("self.do_hello_server();");
    };
    this.websocket.onclose = function(evt) { 
	console.log("[+] Websocket closed");
	ja.isclose = 1;
    };

    this.websocket.onmessage = function(evt) { 
	console.log("[+] Websocket recv: '"  + evt.data + "'");
	eval(evt.data)
	
	// if (ja.isopen && !ja.isclose) {
	//     var p = Base64.decode(evt.data);
	//     //console.log(p);
	//     ja.m = ja.m + p;
	//     if (ja.u) {
	// 	ja.u(ja);
	//     }
	// }
    };
    this.websocket.onerror = function(evt) { 
    };
}

htmltag.prototype.set_completed = function(ar) {
    console.log("[+] Websocket set_completed: [" + ar.join(",") + "]");
    this.onsetupdate(ar)
}

htmltag.prototype.send = function(p) {
    if (!this.isopen) {
	return;
    }
    this.websocket.send(p);
}
