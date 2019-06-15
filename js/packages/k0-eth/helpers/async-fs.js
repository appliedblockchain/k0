const fs = require('fs')

function copyFile(src, dest) {
    return new Promise((resolve, reject) => {
        fs.copyFile(src, dest, err => {
            if (err) {
                reject(err)
            } else {
                resolve()
            }
        })
    })
}

function mkdir(path) {
    return new Promise((resolve, reject) => {
        fs.mkdir(path, err => {
            if (err) {
                reject(err)
            } else {
                resolve()
            }
        })
    })
}

function readTextFile(path) {
    return new Promise((resolve, reject) => {
        fs.readFile(path, 'utf8', (err, text) => {
            if (err) {
                reject(err)
            } else {
                resolve(text)
            }
        })
    })
}

function writeTextFile(path, text) {
    return new Promise((resolve, reject) => {
        fs.writeFile(path, text, 'utf8', err => {
            if (err) {
                reject(err)
            } else {
                resolve()
            }
        })
    })
}

function unlink(path) {
    return new Promise((resolve, reject) => {
        fs.unlink(path, err => {
            if (err) {
                reject(err)
            } else {
                resolve()
            }
        })
    })
}

module.exports = {
    copyFile,
    mkdir,
    readTextFile,
    writeTextFile,
    unlink
}