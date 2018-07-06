var addon = require('bindings')('addon');
let res;

res = addon.addSync(1, 3);
console.log('addSync: 1+3->', res);

try {
    addon.addSync(1, null);
} catch (error) {
    console.log('addSync: should throw error->', error.message)
}

addon.wait()
.then(() => {
    console.log('waited for 1 second')
})
