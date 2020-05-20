console.log('utils.js');

const name = 'Utils Name';

// module.exports = name; // module.exports file is available as the return value while requiring the module

const add = (a,b) => {
    console.log(a+b);
}

module.exports = {
    "add": add,
    "name": name,
}
