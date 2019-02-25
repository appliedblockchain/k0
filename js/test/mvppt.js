'use strict'

const _ = require('lodash')
const assert = require('assert')
const asyncFs = require('../async-fs')
const compileContract = require('./helpers/compile-contract')
const crypto = require('crypto')
const deploy = require('../deploy')
const execAsync = require('../exec-async')
const jayson = require('jayson/promise')
const path = require('path')
const util = require('./util')
const expect = require('code').expect
const sendTransaction = require('../send-transaction')
const vkFromFile = require('../vk-from-file')
const vkToSol = require('../vk-to-sol')

if (process.env.MOCHA_MERKLE_TREE_HEIGHT === undefined) {
    console.log('Env var MOCHA_MERKLE_TREE_HEIGHT must be set.')
    process.exit(1)
}

const TREE_HEIGHT = parseInt(process.env.MOCHA_MERKLE_TREE_HEIGHT || '2', 10)
const NUM_PEOPLE = 2

const tmpDir = '/tmp'

const paths = {
    commitmentPk: path.join(tmpDir, 'zktrade_commitment_pk'),
    commitmentVkAlt: path.join(tmpDir, 'zktrade_commitment_vk_alt'),
    additionPk: path.join(tmpDir, 'zktrade_addition_pk'),
    additionVkAlt: path.join(tmpDir, 'zktrade_addition_vk_alt'),
    withdrawalPk: path.join(tmpDir, 'zktrade_withdrawal_pk'),
    withdrawalVkAlt: path.join(tmpDir, 'zktrade_withdrawal_vk_alt')
}

function extractContractArtefacts(result, contractName) {
    const contractInfo = result.contracts[`${contractName}.sol:${contractName}`]
    return {
        abi: JSON.parse(contractInfo.abi),
        bytecode: '0x' + contractInfo.bin
    }
}

async function generateVerifierContractAlt(pathToVk, filePath, contractName) {
    // Read vk and Solidity contract source code template
    const templateFilePath = path.join(path.parse(module.filename).dir, '..', '..', 'sol', 'GenericVerifier.sol')
    const [vk, contractTemplate] = await Promise.all([
        vkFromFile(pathToVk),
        asyncFs.readTextFile(templateFilePath)
    ])

    // Put contract name and vk into contract source, write to temporary file
    const vkSolSnippet = vkToSol(...vk)
    let contractSource = contractTemplate.replace('____CONTRACT_NAME____', contractName)
    contractSource = contractSource.replace('____VERIFYING_KEY_BODY____', vkSolSnippet)

    await asyncFs.writeTextFile(filePath, contractSource, 'utf8')

}

async function compileContracts() {
    const contractsDir = path.join(tmpDir, crypto.randomBytes(32).toString('hex'))
    await asyncFs.mkdir(contractsDir)
    await generateVerifierContractAlt(
        paths.commitmentVkAlt,
        path.join(contractsDir, 'CommitmentVerifier.sol'),
        'CommitmentVerifier'
    )
    await generateVerifierContractAlt(
        paths.additionVkAlt,
        path.join(contractsDir, 'AdditionVerifier.sol'),
        'AdditionVerifier'
    )
    await generateVerifierContractAlt(
        paths.withdrawalVkAlt,
        path.join(contractsDir, 'WithdrawalVerifier.sol'),
        'WithdrawalVerifier'
    )
    const mtPath = path.join(contractsDir, 'MVPPT.sol')
    await asyncFs.copyFile(
        path.join(
            path.parse(module.filename).dir, '..', '..', 'sol', 'MVPPT.sol'
        ),
        mtPath
    )
    const pairingPath = path.join(contractsDir, 'Pairing.sol')
    await asyncFs.copyFile(
        path.join(
            path.parse(module.filename).dir, '..', '..', 'sol', 'Pairing.sol'
        ),
        pairingPath
    )

    const ozDir = path.join(
        __dirname,
        '..',
        'node_modules',
        'openzeppelin-solidity'
    )
    const outputPath = path.join(contractsDir, 'output.json')
    const command = [
        `solc --combined-json abi,bin openzeppelin-solidity=${ozDir}`,
        `MVPPT.sol > output.json`
    ].join(' ')
    const options = {
        cwd: contractsDir,
        maxBuffer: 1024 * 1024
    }
    const {stdout, stderr} = await execAsync(command, options)
    if (stderr) {
        console.log(stderr)
    }
    const json = await asyncFs.readTextFile(outputPath)
    // TODO Delete directory
    //await asyncFs.unlink(tmpFilePath)
    const result = JSON.parse(json)
    console.log(result)
    return {
        MVPPT: extractContractArtefacts(result, 'MVPPT'),
        AdditionVerifier: extractContractArtefacts(result, 'AdditionVerifier'),
        CommitmentVerifier: extractContractArtefacts(result, 'CommitmentVerifier'),
        WithdrawalVerifier: extractContractArtefacts(result, 'WithdrawalVerifier')
    }
}

async function deployStandardContract(web3, contractName, account = null) {
    const artefacts = await compileContract(contractName)
    const contractAddress = await deploy(
        web3,
        artefacts.abi,
        artefacts.bytecode,
        50000000,
        [],
        account
    )
    return new web3.eth.Contract(artefacts.abi, contractAddress)
}

const wait = ms => new Promise(resolve => setTimeout(resolve, ms))

async function mtEngineReady(mtEngine) {
    let ready = false;

    process.stdout.write('Waiting for the server to become ready...')
    // while (!ready) {
    //   const statusResponse = await mtEngine.request('status', [])
    //   ready = statusResponse.result.ready
    //   if (!ready) {
    //     process.stdout.write('.')
    //     await wait(1000)
    //   }
    // }
    process.stdout.write('\n')
    console.log('Server ready.')
}

function randomBytesHex(len = 32) {
    return '0x' + crypto.randomBytes(len).toString('hex')
}

describe('Minimum viable private payment token', function () {

    this.timeout(4 * 3600 * 1000)
    let web3, erc20, MVPPT, tokenMaster, depositors, mtEngine

    async function printBalances(addresses) {
        addresses = [MVPPT._address, ...addresses]
        console.log('Balances:')
        for (let i = 0; i < addresses.length; i++) {
            const balance = await erc20.methods.balanceOf(addresses[i]).call()
            console.log(addresses[i], web3.utils.fromWei(balance))
        }
    }

    before(async () => {

        web3 = util.initWeb3()
        tokenMaster = web3.eth.accounts.create()
        depositors = _.times(NUM_PEOPLE, () => web3.eth.accounts.create())

        erc20 = await deployStandardContract(web3, 'DollarCoin', tokenMaster)
        const moneyShower = await deployStandardContract(web3, 'MoneyShower')

        // Make some money
        const data = erc20.methods
            .mint(tokenMaster.address, web3.utils.toWei('1000000'))
            .encodeABI()

        await sendTransaction(web3, erc20._address, data, 5000000, tokenMaster)

        // Money to the people
        await sendTransaction(
            web3,
            erc20._address,
            erc20.methods.approve(
                moneyShower._address, web3.utils.toWei((NUM_PEOPLE).toString())
            ).encodeABI(),
            5000000,
            tokenMaster
        )
        await sendTransaction(
            web3,
            moneyShower._address,
            moneyShower.methods.transfer(
                erc20._address,
                _.map(depositors, 'address'),
                _.times(depositors.length, () => "10000000000")
            ).encodeABI(),
            5000000,
            tokenMaster
        )
        const proverPort = parseInt(process.env.PROVER_PORT || '4000', 10)
        mtEngine = jayson.client.http({port: proverPort})
        await mtEngineReady(mtEngine)
        await mtEngine.request('reset', [])
        console.log("RESETTED")
        const initialRootResponse = await mtEngine.request('root', [])
        const initialRoot = initialRootResponse.result
        web3 = util.initWeb3()
        const contractArtefacts = await compileContracts()
        console.log(contractArtefacts)
        const commitmentVerifierAddress = await deploy(
            web3,
            contractArtefacts.CommitmentVerifier.abi,
            contractArtefacts.CommitmentVerifier.bytecode,
            50000000
        )
        const additionVerifierAddress = await deploy(
            web3,
            contractArtefacts.AdditionVerifier.abi,
            contractArtefacts.AdditionVerifier.bytecode,
            50000000
        )
        const withdrawalVerifierAddress = await deploy(
            web3,
            contractArtefacts.WithdrawalVerifier.abi,
            contractArtefacts.WithdrawalVerifier.bytecode,
            50000000
        )
        const MVPPTAddress = await deploy(
            web3,
            contractArtefacts.MVPPT.abi,
            contractArtefacts.MVPPT.bytecode,
            50000000,
            [
                erc20._address,
                commitmentVerifierAddress,
                additionVerifierAddress,
                withdrawalVerifierAddress,
                await util.pack256Bits(initialRoot)
            ]
        )
        MVPPT = new web3.eth.Contract(
            contractArtefacts.MVPPT.abi,
            MVPPTAddress
        )

    })

    it('deposit and withdrawal works', async function () {
        const secrets = [],
            commitmentProvingTimes = [],
            withdrawalProvingTimes = []

        printBalances(_.map(depositors, 'address'))

        for (let i = 0; i < NUM_PEOPLE; i++) {
            const account = depositors[i]

            const v = (_.random(1, 99) * 100000000).toString()
            await sendTransaction(
                web3,
                erc20._address,
                erc20.methods.approve(MVPPT._address, v).encodeABI(),
                5000000,
                account
            )

            const a_sk = randomBytesHex(32)

            const aPkResponse = await mtEngine.request('prf_addr', [a_sk])
            const a_pk = aPkResponse.result

            const rho = randomBytesHex(32)
            const r = randomBytesHex(48)
            secrets[i] = [a_sk, rho, r, v]

            const timestampStart = Date.now()
            const response = await mtEngine.request('prepare_deposit', [a_pk, rho, r, v])
            const proofDuration = Date.now() - timestampStart

            const contractRoot = await Promise.all([
                MVPPT.methods.root(0).call(),
                MVPPT.methods.root(1).call()
            ])

            console.log('getting root from server...')
            const serverRootResponse = await mtEngine.request('root', [])
            console.log('got root from server')
            console.log(serverRootResponse)
            const serverRoot = await util.pack256Bits(serverRootResponse.result)
            console.log("ROOOOOOOOOOOOOTS")
            console.log(contractRoot[0], serverRoot[0])
            console.log(contractRoot[1], serverRoot[1])


            console.log(response)


            const data = response.result;

            console.log(data.commitmentProof)
            const commitmentProofCompact = [
                data.commitmentProof[0][0], // 0 a
                data.commitmentProof[0][1], // 1

                data.commitmentProof[1][0], // 2 a_p
                data.commitmentProof[1][1], // 3

                data.commitmentProof[2][0][0], // 4 b (0)
                data.commitmentProof[2][0][1], // 5
                data.commitmentProof[2][1][0], // 6 b (1)
                data.commitmentProof[2][1][1], // 7

                data.commitmentProof[3][0], // 8 b_p
                data.commitmentProof[3][1], // 9

                data.commitmentProof[4][0], // 10 c
                data.commitmentProof[4][1], // 11

                data.commitmentProof[5][0], // 12 c_p
                data.commitmentProof[5][1], // 13

                data.commitmentProof[6][0], // 14 h
                data.commitmentProof[6][1], // 15

                data.commitmentProof[7][0], // 16 k
                data.commitmentProof[7][1]  // 17
            ]

            const additionProofCompact = [
                data.additionProof[0][0], // 0 a
                data.additionProof[0][1], // 1

                data.additionProof[1][0], // 2 a_p
                data.additionProof[1][1], // 3

                data.additionProof[2][0][0], // 4 b (0)
                data.additionProof[2][0][1], // 5
                data.additionProof[2][1][0], // 6 b (1)
                data.additionProof[2][1][1], // 7

                data.additionProof[3][0], // 8 b_p
                data.additionProof[3][1], // 9

                data.additionProof[4][0], // 10 c
                data.additionProof[4][1], // 11

                data.additionProof[5][0], // 12 c_p
                data.additionProof[5][1], // 13

                data.additionProof[6][0], // 14 h
                data.additionProof[6][1], // 15

                data.additionProof[7][0], // 16 k
                data.additionProof[7][1]  // 17
            ]

            const params = [
                v,
                await util.pack256Bits(data.k),
                await util.pack256Bits(data.cm),
                await util.pack256Bits(data.nextRoot),
                commitmentProofCompact,
                additionProofCompact
            ]
            console.log({params})

            const txData = MVPPT.methods.deposit(
                ...params
            ).encodeABI()
            const receipt = await sendTransaction(web3, MVPPT._address, txData, 5000000, account)
            console.log("yo")

            console.log(receipt)
            assert(receipt.status)

            const depositResponse = await mtEngine.request('add', [data.cm])
            console.log(`Added leaf ${i}: ${data.cm}`)
            console.log(`New root: ${depositResponse.result.newRoot}, should be ${data.nextRoot}`)
            console.log(await util.pack256Bits(depositResponse.result.newRoot))
            assert(depositResponse.result.newRoot === data.nextRoot)
            console.log(`New root: ${depositResponse.result.newRoot}`)
            commitmentProvingTimes.push(proofDuration)

            const timesSum = commitmentProvingTimes.reduce((acc, val) => acc + val)
            const avg = timesSum / commitmentProvingTimes.length

            console.log([
                'Duration of commitment proving operation:',
                `${Math.round(proofDuration / 1000)}s`,
                `(avg: ${Math.round(avg / 1000)}s)`
            ].join(' '))

            await printBalances(_.map(depositors, 'address'))
            console.log()
        }

        const withdrawers = _.times(NUM_PEOPLE, () => web3.eth.accounts.create())

        for (let i = 0; i < NUM_PEOPLE; i++) {

          const account = withdrawers[i];

          const [a_sk, rho, r, v] = secrets[i]
          const timestampStart = Date.now()
          const params = [i.toString(10), a_sk, rho, r, v, account.address]
            console.log("PARAMAS", params)
            console.log(account)
          const withdrawalProofResponse = await mtEngine.request(
              'prepare_withdrawal', params
          )
          const proofDuration = Date.now() - timestampStart
          const { sn, proof } = withdrawalProofResponse.result
          const snPacked = await util.pack256Bits(sn)
          const x = MVPPT.methods.withdraw(v, snPacked, ...proof)
          const data = x.encodeABI()
          const receipt = await sendTransaction(web3, MVPPT._address, data, 5000000, account)
          console.log("Withdrawal successful?", receipt.status)
            console.log(receipt.logs    )
          await printBalances(_.map([...depositors, ...withdrawers], 'address'))
          withdrawalProvingTimes.push(proofDuration)

          const timesSum = withdrawalProvingTimes.reduce((acc, val) => acc + val)
          const avg = timesSum / withdrawalProvingTimes.length

          console.log([
            'Duration of withdrawal proving operation:',
            `${Math.round(proofDuration/1000)}s`,
            `(avg: ${Math.round(avg / 1000)}s)`
          ].join(' '))
          console.log()
        }

    })


})