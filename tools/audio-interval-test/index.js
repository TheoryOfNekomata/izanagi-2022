function AudioForm(el) {
	const interval = el.elements['interval']
	const play = el.elements['play']
	const output = el.querySelector('output')
	const audioPool = []
	let intervalHandle = null
	let audioIndex = 0
	const items = 50

    /*
	for (let i = 0; i < items; i += 1) {
		const newAudio = new Audio('./sfx/weapons/guns/smg-hkmp7/1_unload_var0.wav')
		newAudio.volume = 0.2;
		newAudio.addEventListener('canplaythrough', () => {
			audioPool[i].ready = true
		})
		newAudio.addEventListener('ended', () => {
			newAudio.pause()
			newAudio.currentTime = 0
		})

		audioPool[i] = {
			audio: newAudio,
			ended: true,
			ready: false,
		}
		newAudio.load()
	}
    */
    let started = false;
    window.addEventListener('click', () => {
        if (started) {
            return;
        }
        started = true;
        const audioContext = new AudioContext();

        const gain = audioContext.createGain();
        gain.gain.setValueAtTime(0, audioContext.currentTime);

        const oscillator = audioContext.createOscillator();

        oscillator.type = 'square';
        oscillator.frequency.setValueAtTime(1000, audioContext.currentTime);
        oscillator.start();

        oscillator.connect(gain);
        gain.connect(audioContext.destination);

    	const playSound = () => {
            gain.gain.setValueAtTime(0.25, audioContext.currentTime);
            setTimeout(() => {
                gain.gain.setValueAtTime(0, audioContext.currentTime);
            }, 10);
            /*
    		for (let i = 0; i < items; i += 1) {
    			if (!audioPool[i].ready) {
    				return;
    			}
    		}

    		audioPool[audioIndex].audio.play();
    		audioIndex = (audioIndex + 1) % items;
    */
    	}

    	el.addEventListener('submit', (e) => {
    		e.preventDefault()

    		const ms = 60000 / Number(interval.value);
    		output.innerText = `${ms} ms`
    		if (play.checked) {
    			intervalHandle = window.setInterval(() => {
    				playSound()
    			}, ms)
    		} else {
                gain.gain.setValueAtTime(0, audioContext.currentTime);
    			window.clearInterval(intervalHandle);
    		}

    		play.checked = !play.checked
    	})
    })
}

const audioForm = new AudioForm(window.document.getElementById('formAudio'))
