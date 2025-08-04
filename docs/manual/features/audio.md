# Audio system

An audio system is an essential part of the application experience especially in XR devices, this kind of device is lack of the physical feedbacks like the smartphone or the desktop computer, so the audio feedbacks are more important in the XR applications.

## Audio

JSAR provides the built-in asset loaders for audio files, you can use ECMAScript modules to import the audio files directly in your code:

```js
import soundBuffer from './sound.wav';
```

Then you can use the `Audio` class to create an audio object:

```js
const url = URL.createObjectURL(new Blob([soundBuffer], { type: 'audio/wav' }));
const sound = new Audio(url);
sound.play();
```

Or if your audio file can be accessed via URL, you can use the URL directly:

```js
const sound = new Audio('https://example.com/sound.wav');
sound.play();
```

Or relative to your HTML URL:

```js
const sound = new Audio('./sound.wav');
sound.play();
```

Then you can listen to the events like `ended` or `loadeddata`:

```js
sound.addEventListener('ended', () => {
  console.log('The sound has ended');
});
sound.addEventListener('loadeddata', () => {
  console.info('duration:', audio1.duration);
});
```

Developers can use `document.createElement()` to create an audio element for the audio playback:

```js
const audio = document.createElement('audio');
audio.src = 'https://example.com/sound.wav';
audio.play();
```

## 3D Audio

The [audio spatialization][] is enabled by default for any audio instance.

You don't need to use any Web Audio APIs especially `AudioListener` and `PannerNode` to enable the 3D audio, this is because a document in JSAR represents a subspace in the host space, and each audio instance can be an audio source, the audio system of JSAR can achieve the audio spatialization automatically based on the pose of the each document's subspace.

When users go move around the audio source, or just drag the application in host space, the user can feel where the application by ears.

[audio spatialization]: https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API/Web_audio_spatialization_basics

## Subsystems and APIs to be supported

This section listed the audio-related subsystems and APIs that are planned to be supported in the future.

### Audio session

[Audio Session Spec][Audio Session]:

> This API defines an API surface for controlling how audio is rendered and interacts with other audio playing applications.

With the audio session API, the different applications can work together to mix the audio experiences to users.

[Audio Session]: https://www.w3.org/TR/audio-session/

### Web Audio API

The [Web Audio API][] is a powerful audio processing API that allows developers to create and manipulate audio in the browser, and it also supported the audio streaming playback, the audio effects and the audio spatialization.

We plan to support the Web Audio API in next 2 versions.

[Web Audio API]: https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API
