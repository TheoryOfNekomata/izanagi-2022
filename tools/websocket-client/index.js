const stringifyData = async value => {
	const isString = typeof value === 'string'
	const resolve = () => (
		isString
			? Promise.resolve(value)
			: value.arrayBuffer()
	)

	const a = await resolve()

	if (isString) {
		return `String:${a.length}(${a})`
	}

	const bytes = Array.from(new Uint8Array(a)).map(c => Number(c).toString(16).padStart(2, '0')).join(' ')
	return `Binary:${a.byteLength}(${bytes})`
}

function ConnectionForm(el) {
	let ws = null
	el.addEventListener('submit', e => {
		e.preventDefault()
		if (!ws) {
			const username = el.elements['username'].value
			const baseUrl = el.elements['serverUrl'].value
			ws = new WebSocket(`${baseUrl}?username=${username}`)

			ws.addEventListener('open', () => {
				this.messageForm.enable()
				this.logsForm.append('Connection opened')
				el.elements['serverUrl'].setAttribute('disabled', 'disabled')
				el.elements['username'].setAttribute('disabled', 'disabled')
				el.elements['connect'].innerText = 'Disconnect'
			})

			ws.addEventListener('message', evt => {
				stringifyData(evt.data).then(value => {
					this.logsForm.append(`Recv: ${value}`)
				})
			})

			ws.addEventListener('error', evt => {
				console.error(evt)
			})

			ws.addEventListener('close', event => {
				this.messageForm.reset()
				this.messageForm.disable()
				this.logsForm.append(`Connection closed: wasClean: ${event.wasClean}, evCode: ${event.code}`)
				el.elements['serverUrl'].removeAttribute('disabled')
				el.elements['username'].removeAttribute('disabled')
				el.elements['connect'].innerText = 'Connect'
				ws = null
			})
			return
		}
		ws.close()
	})

	this.send = value => {
		if (!ws) {
			return
		}
		stringifyData(value).then(v => {
			this.logsForm.append(`Send: ${v}`)
			ws.send(value)
		})
	}
}

function MessageForm(el) {
	const inputMessage = el.elements['message']
	const binary = el.elements['binary']

	inputMessage.addEventListener('keydown', e => {
		if (e.key !== 'Shift') {
			return;
		}
		binary.checked = true;
	});

	inputMessage.addEventListener('keyup', e => {
		if (e.key !== 'Shift') {
			return;
		}
		binary.checked = false;
	});

	el.addEventListener('submit', e => {
		e.preventDefault()

		if (inputMessage.value.length < 1) {
			return
		}


		this.connectionForm.send(
			binary.checked
				? new Blob(inputMessage.value.split(''))
				: inputMessage.value,
		);
		inputMessage.value = ''
		inputMessage.focus()
	})

	el.elements['submitBinary'].addEventListener('click', e => {
		e.preventDefault()
		if (inputMessage.value.length < 1) {
			return
		}
		this.connectionForm.send(new Blob(inputMessage.value.split('')))
		inputMessage.value = ''
		inputMessage.focus()
	})

	this.reset = () => {
		inputMessage.value = ''
	}

	this.enable = () => {
		el.elements['fieldset'].removeAttribute('disabled')
	}

	this.disable = () => {
		el.elements['fieldset'].setAttribute('disabled', 'disabled')
	}
}

function LogsForm(el) {
	let logs = el.elements['logs']

	el.addEventListener('submit', e => {
		e.preventDefault()
		logs.value = ''
	})

	this.append = data => {
		logs.value += data + '\n'
		logs.scrollTop = logs.scrollHeight
	}
}

const logsForm = new LogsForm(window.document.getElementById('formLogs'))
const messageForm = new MessageForm(window.document.getElementById('formMessage'))
const connectionForm = new ConnectionForm(window.document.getElementById('formConnection'))

connectionForm.logsForm = logsForm
connectionForm.messageForm = messageForm
messageForm.connectionForm = connectionForm
