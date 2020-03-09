const filters = [
  { vendorId: 0x1A86 }
];
const serialOption = { 
  baudrate: 115200,
  databits:8,
  stopbits:1,
  parity:'none',
  buffersize:128
};

let gElegWriter = false;
const onClickOpenDevice = async (evt) => {
  try {
    const device = await navigator.serial.requestPort({filters: filters});
    console.log('onClickOpenDevice::device=<',device,'>');
    await device.open(serialOption);
    console.log('onClickOpenDevice::device=<',device,'>');
    const reader = device.readable.getReader();
    console.log('onClickOpenDevice::reader=<',reader,'>');

    gElegWriter = device.writable.getWriter();
    console.log('onClickOpenDevice::gElegWriter=<',gElegWriter,'>');

    setTimeout(()=> {
      readSerailData(reader);
    },0);
  }
  catch(e) {
    console.log('onClickListDevice::e=<',e,'>');
  }
}

let readBuffOneLine = '';
let readLineBuffer = [];
const elemRead = document.getElementById('serail-output');
const readSerailData = (reader)=> {
  const readOne = reader.read();
  readOne.then((evt)=>{
    //console.log('readSerailData::readOne evt=<',evt,'>');
    //console.log('readSerailData::readOne evt.value=<',evt.value.buffer,'>');
    const utf8txt = new TextDecoder("utf-8").decode(evt.value);
    //console.log('readSerailData::readOne utf8txt=<',utf8txt,'>');
    readBuffOneLine += utf8txt;
    const hint = utf8txt.indexOf('&$');
    if(hint) {
      //console.log('readSerailData::readOne readBuffOneLine=<',readBuffOneLine,'>');
      readLineBuffer.push(readBuffOneLine);            
      if(readLineBuffer.length > 20) {
        readLineBuffer.shift();
      }            
      elemRead.textContent = readLineBuffer.join('');
      readBuffOneLine = '';
    }
    if(evt.done === false) {
      setTimeout(()=>{
        readSerailData(reader);
      },0)
    }
  });          
};


const onClickCalibratePosition = (evt) => {
  try {
  if(gElegWriter) {
    const reqcalibrate = 'calibrate\n';
    const wBuff = new TextEncoder().encode(reqcalibrate);
    console.log('onClickCalibratePosition::wBuff=<',wBuff,'>');
    gElegWriter.write(wBuff);
  }
  }
  catch(e) {
    console.log('onClickCalibratePosition::e=<',e,'>');
  }
}


const onClickChangePosition = (elem) => {
  try {
    changeRangevalue(elem.value);
    if(gElegWriter) {
      console.log('onClickChangePosition::elem.value=<',elem.value,'>');
      const reqPosition = `pos:#{elem.value}\n`;
      const wBuff = new TextEncoder().encode(reqPosition);
      console.log('onClickChangePosition::wBuff=<',wBuff,'>');
      gElegWriter.write(wBuff);
    }
  }
  catch(e) {
    console.log('onClickChangePosition::e=<',e,'>');
  }
}

const changeRangevalue = (value) => {
  console.log('changeRangevalue::value=<',value,'>');
  const valueElem = document.getElementById('eleg-position-value');
  valueElem.textContent = value;
}

/*
const onClickOpenEccKeyDevice = async (evt) => {
  try {
    const device = await navigator.serial.requestPort({filters: filters});
    console.log('onClickOpenEccKeyDevice::device=<',device,'>');
    await device.open(serialOption);
    console.log('onClickOpenEccKeyDevice::device=<',device,'>');
    const reader = device.readable.getReader();
    console.log('onClickOpenEccKeyDevice::reader=<',reader,'>');
    setTimeout(()=> {
      readEccDeviceData(reader);
    },0);
    gEccDeviceWriter = device.writable.getWriter();;
    console.log('onClickOpenEccKeyDevice::gEccDeviceWriter=<',gEccDeviceWriter,'>');
  }
  catch(e) {
    console.log('onClickOpenEccKeyDevice::e=<',e,'>');
  }
}


const elemReadEcc = document.getElementById('serail-output');
const readEccDeviceData = (reader)=> {
  const readOne = reader.read();
  readOne.then((evt)=>{
    //console.log('readEccDeviceData::readOne evt=<',evt,'>');
    //console.log('readEccDeviceData::readOne evt.value=<',evt.value.buffer,'>');
    const utf8txt = new TextDecoder('utf-8').decode(evt.value);
    //console.log('readEccDeviceData::readOne utf8txt=<',utf8txt,'>');
    elemReadEcc.textContent += utf8txt;
    if(evt.done === false) {
      setTimeout(()=>{
        readEccDeviceData(reader);
      },0)
    }
  });          
};

let gEccDeviceWriter = false;
const onClickReadPublicKey = ()=> {
  if(gEccDeviceWriter) {
    const reqPubKey = {f:'pub'};
    const wBuff = new TextEncoder().encode(JSON.stringify(reqPubKey));
    console.log('onClickReadPublicKey::wBuff=<',wBuff,'>');
    gEccDeviceWriter.write(wBuff);
  }
}
*/
