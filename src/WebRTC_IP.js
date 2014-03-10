#include <Wt/WJavaScriptPreamble>

const std::string WebRTC_IP_JS = WT_JS(

// https://hacking.ventures/local-ip-discovery-with-html5-webrtc-security-and-privacy-risk/
// https://dl.dropboxusercontent.com/u/1878671/enumhosts.html

function TaskController(numConcurrent, onDone) {
  this.numConcurrent = numConcurrent;
  this.onDone = onDone || function() {};
  this.pending = 0;
  this.queued = [];
  this.checkTimer = -1;
}

TaskController.prototype.deferCheck = function() {
  if (this.checkTimer != -1) return;
  this.checkTimer = setTimeout((function() {
    this.checkTimer = -1;
    this.check();
  }).bind(this), 0);
};

TaskController.prototype.check = function() {
  if (this.pending < 1 && this.queued.length == 0) return this.onDone();
  while (this.pending < this.numConcurrent && this.queued.length > 0) {
    try {
      this.pending += 1;
      setTimeout((function(task) {
        task((function() {
          this.pending -= 1;
          this.deferCheck();
        }).bind(this));
      }).bind(this, this.queued.shift()), 0);
    }
    catch (e) {
      this.pending -= 1;
      this.deferCheck();
    }
  }
};

TaskController.prototype.queue = function(task) {
  this.queued.push(task);
  this.deferCheck();
};

function probeIp(ip, timeout, cb) {
  var start = Date.now();
  var done = false;
  var img = document.createElement('img');
  var clean = function() {
    if (!img) return;
    document.body.removeChild(img);
    img = null;
  };
  var onResult = function(success) {
    if (done) return;
    done = true;
    clean();
    cb(ip, Date.now() - start < timeout);
  };
  document.body.appendChild(img);
  img.style.display = 'none';
  img.onload = function() { onResult(true); };
  img.onerror = function() { onResult(false); };
  img.src = 'https://' + ip + ':' + ~~(1024+1024*Math.random()) + '/I_DO_NOT_EXIST?' + Math.random();
  setTimeout(function() { if (img) img.src = ''; }, timeout + 500);
}

function probeNet(net, onHostFound, onDone) {
  net = net.replace(new RegExp("(\\d+\\.\\d+\\.\\d+)\\.\\d+"), '$1.');
  var timeout = 5000;
  var done = false;
  var found = [];
  var q = new TaskController(5, onDone);
  for (var i = 1; i < 256; ++i) {
    q.queue((function(i, cb) {
      probeIp(net + i, timeout, function(ip, success) {
        if (success) onHostFound(ip);
        cb();
      });
    }).bind(this, i));
  }
}

function enumLocalIPs(cb) {
  var RTCPeerConnection = window.webkitRTCPeerConnection || window.mozRTCPeerConnection;
  if (!RTCPeerConnection) return false;
  var addrs = Object.create(null);
  addrs['0.0.0.0'] = false;
  function addAddress(newAddr) {
    if (newAddr in addrs) return;
    addrs[newAddr] = true;
    cb(newAddr);
  }
  function grepSDP(sdp) {
    var hosts = [];
    sdp.split('\r\n').forEach(function (line) {
      if (~line.indexOf('a=candidate')) {
        var parts = line.split(' '),
            addr = parts[4],
            type = parts[7];
        if (type === 'host') addAddress(addr);
      } else if (~line.indexOf('c=')) {
        var parts = line.split(' '),
        addr = parts[2];
        addAddress(addr);
      }
    });
  }
  var rtc = new RTCPeerConnection({iceServers:[]});
  if (window.mozRTCPeerConnection) rtc.createDataChannel('', {reliable:false});
  rtc.onicecandidate = function (evt) {
    if (evt.candidate) grepSDP(evt.candidate.candidate);
  };
  setTimeout(function() {
    rtc.createOffer(function (offerDesc) {
      grepSDP(offerDesc.sdp);
      rtc.setLocalDescription(offerDesc);
    }, function (e) {});
  }, 500);
  return true;
}

function go() {
  var q = new TaskController(1);
  enumLocalIPs(function(localIp) {
    __webrtc_ip__;
    q.queue(function(cb) {
      probeNet(localIp,
               function(ip) {
                 __webrtc_lan__;
               },
               cb);
    });
  });
}

go();

// http://jsfiddle.net/dLVG6/

var ping_array = [];
function measure_ping() {
    var startTime;
    jQuery.ajax({
        url: "http://" + window.location.host +
             "/favicon.ico?cache=" + Math.random(),
        beforeSend: function(){
            startTime = +new Date();
        },
        complete: function(jqXHR, textStatus){
            var data = (+new Date()) - startTime;
            ping_array.push(data);
            if (ping_array.length < 10) {
                measure_ping()
            } else {
                ping_array.sort(function(a, b) { return a - b; });
                var ping = ping_array[5]; // median
                __ping__;
            }
        }
    });
}
measure_ping();

);

