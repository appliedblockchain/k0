'use strict'

function awaitEvent(emitter, eventName, timeoutSecs = 20) {
  return new Promise((accept, reject) => {
    const timeout = setTimeout(() => {
      // TODO unregister the event listener?
      reject(new Error(`Event was not emitted within ${timeoutSecs} seconds`))
    }, timeoutSecs * 1000)
    emitter.once(eventName, (event) =>{
      accept(event)
      clearTimeout(timeout)
    })
  })
}

module.exports = {
  awaitEvent
}
