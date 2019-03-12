function parseG1Point(data) {
	const X = data[0]
	const Y = data[1]
	return [X, Y]
}

function parseG2Point(data) {
	const X = [data[1], data[0]]
	const Y = [data[3], data[2]]
	return [X, Y]
}

const wait = ms => new Promise(resolve => setTimeout(resolve, ms))

module.exports = {
  parseG1Point,
  parseG2Point,
	wait
}
