let { expect, done } = require('chai')
const SaharaRenodeLibrary = require('../index.js')
const SaharaRenodeFactory = SaharaRenodeLibrary.SaharaRenodeFactory

function sleep(ms) {
  return new Promise((resolve) => {
    setTimeout(resolve, ms)
  })
}

describe('sahara renode', () => {
  // basic tests
  describe('config test', () => {
    let config = {
        verbose: true,
        port: 12348,
    }

    // grab an instance
    let SaharaRenode0 = SaharaRenodeFactory(0)
    SaharaRenode0.setConfig(config)

    it.skip('Start Renode', async function () {
      SaharaRenode0.test();
    })

    it('Start Renode', async function () {
      //start renode
      const res1 = SaharaRenode0.start()
      console.log(res1)
      expect(res1).to.equal(true)

      const res2 = SaharaRenode0.getRenodeResponse()
      console.log(res2)
    })

    it('load script', async function (){
      const lines =[
        ':name: Mi-V',
        ':description: This script runs Zephyr RTOS shell sample on a Mi-V RISC-V board.',
        '$name?="Mi-V"',        
        'using sysbus',
        'mach create $name',
        'machine LoadPlatformDescription @platforms/boards/miv-board.repl',        
        '$bin?=@https://dl.antmicro.com/projects/renode/shell-demo-miv.elf-s_803248-ea4ddb074325b2cc1aae56800d099c7cf56e592a',        
        'showAnalyzer uart',        
        'macro reset',
        '"""',
        '    sysbus LoadELF $bin',
        '"""',
        'runMacro $reset'
      ]
      const res = SaharaRenode0.loadScript(lines)      
      console.log(res)
      expect(res).to.equal(true)
    })

    it.skip('Set Machine', async function () {
      const res = SaharaRenode0.setActiveMachine("Mi-V")
      expect(res).to.equal(true)

      //get active machine
      const activeMachine = SaharaRenode0.getActiveMachine();
      expect(activeMachine).to.equal("Mi-V")
    })

    it.skip('Get Peripherals', async function () {
      const res = SaharaRenode0.getPeripherals("Mi-V")
      console.log(res)
    })

    it.skip('Get Peripheral', async function () {
      const res = SaharaRenode0.getPeripheral("Mi-V", "sysbus")
      console.log(res)
    })

    it.skip('Get Sub Peripheral', async function () {
      const res = SaharaRenode0.getPeripheral("Mi-V", "sysbus.cpu")
      console.log(res)
    })

    it('Get Peripheral property', async function () {
      const res = SaharaRenode0.readProperty("Mi-V", "sysbus", "Endianess")
      console.log(res)
    })

    it('Set Peripheral property', async function () {
      const res = SaharaRenode0.setProperty("Mi-V", "sysbus", "UnhandledAccessBehaviour", "Report");
      console.log(res)
    })

    it('Call Peripheral method', async function () {
      const res = SaharaRenode0.callMethod("Mi-V", "sysbus", "GetName", []);
      console.log(res)
    })



    it.skip('start emulator', async function (){
      const res = SaharaRenode0.startEmulator()
      expect(res).to.equal(true)
      console.log(res)
    })    

    it.skip('pause emulator', async function (){
      const res = SaharaRenode0.pauseEmulator()
      expect(res).to.equal(true)
      console.log(res)
    })

    it('quit renode', async function (){
      const res = SaharaRenode0.stop()
      expect(res).to.equal(true)
    })
    
  })
})
