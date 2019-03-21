const makeLinkedList = require('../../state/linked-list')
const expect = require('code').expect

describe('linked list', () => {
  it('works', () => {
    const list = makeLinkedList()
    const first = { foo: 'bar' }
    list.add(first)
    const second = { abc: 'xyz' }
    list.add(second)
    const it = list.iterator()
    expect(it.next()).to.equal({ value: second, done: false })
    expect(it.next()).to.equal({ value: first, done: false })
    expect(it.next()).to.equal({ done: true })
  })
})