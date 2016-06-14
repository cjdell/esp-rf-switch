const char SWITCH_SETUP_HTML[] PROGMEM = "<!doctype html>\n<html>\n\n<head>\n  <meta charset=\"utf-8\" />\n  <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\" />\n  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />\n\n  <title>RF Switch Setup</title>\n\n  <style type=\"text/css\">\n    body {\n      font-family: Arial, Helvetica, sans-serif;\n      box-sizing: border-box;\n    }\n\n    h1 {\n      text-align: center;\n    }\n\n    .page {\n      max-width: 480px;\n      margin: 0 auto;\n    }\n\n    .device-list {\n      list-style: none;\n      margin: 0;\n      padding: 0;\n    }\n\n    .device {\n      display: flex;\n    }\n\n    .device.template {\n      display: none;\n    }\n\n    .device > * {\n      flex: 2;\n      padding: 2px 4px;\n      height: 24px;\n      border: solid 2px #aaa;\n      margin: 5px;\n      border-radius: 3px;\n      width: 0;\n    }\n\n    .device a {\n      flex: 1;\n      display: block;\n      text-align: center;\n      line-height: 24px;\n      text-decoration: none;\n      color: #000;\n    }\n\n    .device a:hover {\n      background: #ddd;\n    }\n\n    .addDevice,\n    .save {\n      display: block;\n      text-align: center;\n      line-height: 24px;\n      text-decoration: none;\n      color: #000;\n      border: solid 2px #aaa;\n      border-radius: 3px;\n      margin: 10px 5px;\n    }\n  </style>\n\n  <script type=\"text/javascript\">\n    var deviceTemplate;\n    var loadedDevices;\n\n    window.addEventListener('load', function() {\n      var addDeviceButton = document.querySelector('.addDevice');\n      var saveButton = document.querySelector('.save');\n\n      deviceTemplate = document.querySelector('.device.template');\n      deviceTemplate.classList.remove('template');\n      deviceTemplate.remove();\n\n      loadDevices();\n\n      addDeviceButton.addEventListener('click', addDevice);\n      saveButton.addEventListener('click', function() {\n        loadedDevices = parseDevices();\n        saveDevices(loadedDevices);\n      });\n    });\n\n    function loadDevices() {\n      window.fetch('/devices.json').then(function(res) {\n        if (res.status !== 200) throw new Error(res.statusText);\n        return res.json();\n      }).then(function(devices) {\n        loadedDevices = devices;\n        showDevices(loadedDevices);\n      }).catch(function(err) {\n        if (err instanceof SyntaxError) {\n          loadedDevices = [];\n          showDevices(loadedDevices);\n        }\n        console.error(err);\n        alert('Error loading: ' + err.message);\n      });\n    }\n\n    function showDevices(devices) {\n      var list = document.querySelector('.device-list');\n\n      list.innerHTML = '';\n\n      devices.forEach(function(device) {\n        var deviceNode = deviceTemplate.cloneNode(true);\n        var nameInput = deviceNode.querySelector('.name');\n        var codeInput = deviceNode.querySelector('.code');\n        var detectButton = deviceNode.querySelector('.detect');\n        var onButton = deviceNode.querySelector('.on');\n        var offButton = deviceNode.querySelector('.off');\n\n        nameInput.value = device.name;\n        codeInput.value = device.code;\n\n        list.appendChild(deviceNode);\n\n        detectButton.addEventListener('click', detectDevice.bind(null, device, deviceNode));\n\n        onButton.addEventListener('click', function() {\n          sendOnOff(codeInput.value, true);\n        });\n\n        offButton.addEventListener('click', function() {\n          sendOnOff(codeInput.value, false);\n        });\n      });\n    }\n\n    function parseDevices() {\n      var list = document.querySelector('.device-list');\n\n      var devices = [].map.call(list.children, function(deviceNode) {\n        var nameInput = deviceNode.querySelector('.name');\n        var codeInput = deviceNode.querySelector('.code');\n\n        return {\n          name: nameInput.value,\n          code: codeInput.value\n        };\n      });\n\n      return devices;\n    }\n\n    function addDevice() {\n      loadedDevices = parseDevices();\n      loadedDevices.push({ name: '', code: '' });\n      showDevices(loadedDevices);\n    }\n\n    function saveDevices(devices) {\n      window.fetch('/devices.json?json=' + encodeURIComponent(JSON.stringify(devices)), {\n        method: 'post',\n        // body: JSON.stringify(devices)\n      }).then(function(res) {\n        if (res.status !== 200) throw new Error(res.statusText);\n        alert('Save successful');\n      }).catch(function(err) {\n        console.error(err);\n        alert('Error saving: ' + err.message);\n      });\n    }\n\n    function detectDevice(device, deviceNode) {\n      var codeInput = deviceNode.querySelector('.code');\n\n      window.fetch('/listen').then(function(res) {\n        if (res.status !== 200) throw new Error(res.statusText);\n        return res.text();\n      }).then(function(text) {\n        if (text === 'Not available') return;\n\n        var parts = text.split(':');\n        var code = parseInt(parts[0], 10);\n        var bitLength = parts[1];\n\n        code = (code >> 1) << 1;\n\n        codeInput.value = code + ':' + bitLength;\n      }).catch(function(err) {\n        console.error(err);\n        alert('Error detecting: ' + err.message);\n      });\n    }\n\n    function sendOnOff(codeStr, on) {\n      var parts = codeStr.split(':');\n      var code = parseInt(parts[0], 10);\n      var bitLength = parts[1];\n\n      if (on) code += 1;\n\n      codeStr = code + ':' + bitLength;\n\n      window.fetch('/cmd?code=' + codeStr);\n    }\n  </script>\n</head>\n\n<body>\n  <div class=\"page\">\n    <h1>RF Switch Setup</h1>\n\n    <ul class=\"device-list\">\n      <li class=\"device template\">\n        <input type=\"text\" class=\"name\" placeholder=\"Device name\" />\n        <input type=\"text\" class=\"code\" placeholder=\"Code\" />\n        <a href=\"javascript:void(0)\" class=\"detect\" title=\"Detect the last code sent from your remote control\">Detect</a>\n        <a href=\"javascript:void(0)\" class=\"on\">On</a>\n        <a href=\"javascript:void(0)\" class=\"off\">Off</a>\n      </li>\n      <li>\n        <span>Loading...</span>\n      </li>\n    </ul>\n\n    <a href=\"javascript:void(0)\" class=\"addDevice\">Add device</a>\n    <a href=\"javascript:void(0)\" class=\"save\">Save</a>\n  </div>\n</body>\n\n</html>";

void handleRoot() {
  digitalWrite(DEBUG_LED, 1);
  String body = getDeviceName();
  server.send(200, "text/plain", body);
  digitalWrite(DEBUG_LED, 0);
}

void handleSetup() {
  digitalWrite(DEBUG_LED, 1);
  server.send(200, "text/html", SWITCH_SETUP_HTML);
  digitalWrite(DEBUG_LED, 0);
}

void handleQuery() {
  digitalWrite(DEBUG_LED, 1);

  String json = "{\"vendorId\":\"" + String(ESP.getChipId()) + "\",\"name\":\"" + getDeviceName() + "\",\"switches\":" + String(getConfigJson()) + "}";
  
  server.send(200, "text/json", json);
  
  digitalWrite(DEBUG_LED, 0);
}

void handleWriteDevices() {
  String code = server.arg("json");

  EEPROM.write(0, (uint8_t)code.length());

  Serial.println(String("Writting EEPROM ") + code.length());
  
  for (size_t i = 0; i < code.length(); i++) {
    EEPROM.write(i + 1, (uint8_t)code[i]); 
  }

  EEPROM.commit();

  server.send(200, "text/json", "Written");
}

void handleReadDevices() {
  String buf = getConfigJson();

  server.send(200, "text/json", buf);
}

void handleCmd() {
  digitalWrite(DEBUG_LED, 1);

  String codeStr = server.arg("code");
  int sep = codeStr.indexOf(':');
  int code = codeStr.substring(0, sep).toInt();
  int bitLength = codeStr.substring(sep + 1).toInt();
  
  rcSwitch.send(code, bitLength);
  
  server.send(200, "text/plain", String("ACK ") + code);
  digitalWrite(DEBUG_LED, 0);
}

void handleCmd2() {
  digitalWrite(DEBUG_LED, 1);

  String codeStr = server.arg("code");
  
  rcSwitch.send(codeStr.c_str());
  
  server.send(200, "text/plain", String("ACK ") + codeStr);
  digitalWrite(DEBUG_LED, 0);
}

void handleListen() {
  digitalWrite(DEBUG_LED, 1);

  String output = String("");
  
  if (rcSwitch.available()) {
    int value = rcSwitch.getReceivedValue();
    
    if (value == 0) {
      Serial.print("Unknown encoding");
      
      output += "Unknown encoding";
    } else {
      Serial.print("Received ");
      Serial.print( rcSwitch.getReceivedValue() );
      Serial.print(" / ");
      Serial.print( rcSwitch.getReceivedBitlength() );
      Serial.print("bit ");
      Serial.print("Protocol: ");
      Serial.println( rcSwitch.getReceivedProtocol() );

      output += rcSwitch.getReceivedValue();
      output += ":";
      output += rcSwitch.getReceivedBitlength();
    }
  } else {
    output += "Not available";
  }

  rcSwitch.resetAvailable();

  server.send(200, "text/plain", output);
  digitalWrite(DEBUG_LED, 0);
}

void handleNotFound() {
  digitalWrite(DEBUG_LED, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(DEBUG_LED, 0);
}

