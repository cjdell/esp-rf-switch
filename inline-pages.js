const fs = require('fs');

const html = fs.readFileSync('./pages/switch-setup.html', 'utf8');
const escapedHtml = html.replace(/\"/g, '\\"').replace(/\n/g, '\\n');

const arduinoSource = fs.readFileSync('./handlers.ino', 'utf8');

const transformedSource = arduinoSource.split('\n').map(line => {
  if (line.indexOf('const char SWITCH_SETUP_HTML[] PROGMEM') !== -1) {
    return 'const char SWITCH_SETUP_HTML[] PROGMEM = "' + escapedHtml + '";';
  } else {
    return line;
  }
}).join('\n');

fs.writeFileSync('./handlers.ino', transformedSource, 'utf8');
