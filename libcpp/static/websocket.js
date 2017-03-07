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
	ja.websocket.send("start");
    };
    this.websocket.onclose = function(evt) { 
	console.log("[+] Websocket closed");
	ja.isclose = 1;
    };

    this.websocket.onmessage = function(evt) { 
	console.log("[+] Websocket recv: '"  + evt.data + "'");
	eval(evt.data)
	
    };
    this.websocket.onerror = function(evt) { 
    };
}

htmltag.prototype.send = function(p) {
    if (!this.isopen) {
	return;
    }
    this.websocket.send(p);

}

htmltag.prototype.fetchData = function(fn) {
}
