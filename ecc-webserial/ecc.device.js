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
const onClickListDevice = async (evt) => {
  try {
    const device = await navigator.serial.requestPort({filters: filters});
    console.log('onClickListDevice::device=<',device,'>');
    await device.open(serialOption);
    console.log('onClickListDevice::device=<',device,'>');
    const reader = device.readable.getReader();
    console.log('onClickListDevice::reader=<',reader,'>');
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
    //console.log('onClickListDevice::readOne evt=<',evt,'>');
    //console.log('onClickListDevice::readOne evt.value=<',evt.value.buffer,'>');
    const utf8txt = new TextDecoder("utf-8").decode(evt.value);
    //console.log('onClickListDevice::readOne utf8txt=<',utf8txt,'>');
    readBuffOneLine += utf8txt;
    if(utf8txt === '\n') {
      //console.log('onClickListDevice::readOne readBuffOneLine=<',readBuffOneLine,'>');
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
