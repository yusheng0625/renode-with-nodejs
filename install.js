const os = require('os')
var spawn = require('cross-spawn')

// linux and mac
//if (os.platform() === 'linux' || os.platform() === 'darwin') {
console.log('Running C++ build on Linux or Mac...')

// try doing a partial build
let res = spawn.sync('npm', ['run', 'native_no_rebuild'], {
  input: 'Build native module.',
  stdio: 'inherit',
})

// there was an error, so just rebuild the entire thing
if (res.status != 0) {
  // rebuild from scratch
  res = spawn.sync('npm', ['run', 'native_build'], {
    input: 'Build native module.',
    stdio: 'inherit',
  })
}
//}