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
let readLineBufferSlice = [];
let readLineBuffer = [];
const elemRead = document.getElementById('serail-output');
const readSerailData = (reader)=> {
  const readOne = reader.read();
  readOne.then((evt)=>{
    //console.log('readSerailData::readOne evt=<',evt,'>');
    //console.log('readSerailData::readOne evt.value=<',evt.value.buffer,'>');
    const utf8txt = new TextDecoder("utf-8").decode(evt.value);
    //console.log('readSerailData::readOne utf8txt=<',utf8txt,'>');    
    const hint = utf8txt.indexOf('&$');
    const utf8Param = utf8txt.split('&$');
    //console.log('readSerailData::readOne utf8Param=<',utf8Param,'>');
    if(utf8Param.length > 1) {
      readLineBufferSlice.push(utf8Param[0]);
      const oneLineBuf = readLineBufferSlice.join('');
      //console.log('readSerailData::readOne oneLineBuf=<',oneLineBuf,'>');
      onELegInfoLine(oneLineBuf);
      readLineBuffer.push(oneLineBuf);            
      if(readLineBuffer.length > 20) {
        readLineBuffer.shift();
      }
      readLineBufferSlice = [];
      readLineBufferSlice.push(utf8Param[1]);
      elemRead.textContent = readLineBuffer.join('');
      readBuffOneLine = '';
    } else {
      readLineBufferSlice.push(utf8Param[0]);
    }
    if(evt.done === false) {
      setTimeout(()=>{
        readSerailData(reader);
      },0)
    }
  });          
};

const ElegPosCounterStarter = 'iPositionByHallCounter=<';
const ElegPosLowStarter = 'iPositionByHallRangeLow=<';
const ElegPosHightStarter = 'iPositionByHallRangeHigh=<';

const onELegInfoLine = (lineCmd) => {
  console.log('onELegInfoLine::lineCmd=<',lineCmd,'>');
  if( lineCmd.indexOf(ElegPosCounterStarter) >= 0) {
    const currentPos = getValueOfLineCmd(lineCmd);
    console.log('onELegInfoLine::currentPos=<',currentPos,'>');
    updateRangevalue(currentPos);
  }
  if(lineCmd.indexOf(ElegPosLowStarter) >= 0) {
    const lowPos = getValueOfLineCmd(lineCmd);
    console.log('onELegInfoLine::lowPos=<',lowPos,'>');
    changeLowOfPos(lowPos);
  }
  if(lineCmd.indexOf(ElegPosHightStarter) >= 0) {
    const hightPos = getValueOfLineCmd(lineCmd);
    console.log('onELegInfoLine::hightPos=<',hightPos,'>');
    changeHighOfPos(hightPos);
  }
}

const getValueOfLineCmd =(lineCmd) => {
  const start = lineCmd.indexOf('=<');
  const end = lineCmd.indexOf('>');
  const value = lineCmd.slice(start+2,end);
  console.log('onELegInfoLine::value=<',value,'>');
  return parseInt(value);
}

const changeLowOfPos = (value) => {
  const minElem = document.getElementById('eleg-position-min');
  minElem.textContent = value;
  const rangeElem = document.getElementById('eleg-position-value-range');
  rangeElem.setAttribute('min',value);
}


const changeHighOfPos = (value) => {
  const maxElem = document.getElementById('eleg-position-max');
  maxElem.textContent = value;
  const rangeElem = document.getElementById('eleg-position-value-range');
  rangeElem.setAttribute('max',value);
}

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


const onChangePosition = (elem) => {
  try {
    changeRangevalue(elem.value);
    if(gElegWriter) {
      console.log('onClickChangePosition::elem.value=<',elem.value,'>');
      const reqPosition = `pos:${elem.value}\n`;
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
  const targetElem = document.getElementById('eleg-position-value-target-label');
  targetElem.textContent = value;
  updateRangevalue(value);
}

const updateRangevalue = (value) => {
  console.log('updateRangevalue::value=<',value,'>');
  const labelElem = document.getElementById('eleg-position-value-label');
  labelElem.textContent = value;
 
 console.log('updateRangevalue::value=<',value,'>');
  const rangeElem = document.getElementById('eleg-position-value-range');
  rangeElem.value = value;
}
