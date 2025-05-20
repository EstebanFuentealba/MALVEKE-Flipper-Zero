<script>
import { nextTick, onBeforeUnmount, onMounted, ref, defineAsyncComponent } from "vue";
const GBCAM_W = 128;
const GBCAM_H = 112;
const FRAME_JP = "data:image/png;base64,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";
// const FRAME_INT = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAKAAAACQBAMAAAB9m32zAAAAAXNSR0IB2cksfwAAAAlwSFlzAAALEwAACxMBAJqcGAAAAA9QTFRFAAAAVVVV////qqqqAAAA4Pb6uAAAAAV0Uk5T/////wD7tg5TAAABzklEQVR4nO3a7W0kIQwGYHtoAJgGBrsB8LSQ/mvKazaRbqWTIkiU02ltifnUPngw+89E/2twyVQyl7++msdF8JBO1qr+QXxGlYyj9VWwkWiVeXNO4glk1VVQsuhMLScRzadnxE4B5DtjwlWwS6uCs/QqpeJkilmgSzbMlb9GnkG7HqCKJmFwuDO5RE2wBKsJAhz6ALPpKZ4XJJbmD6svxzLY5Qm87+HgnMX2wPoE5jS+m2HybfMBsgxrDl4213BsrGHmjwwrioLN5zLAzyovFvkl4u2bEWCAAQYYYIABBhhggAEGGGCAAQYYYIABBhhggAEGGGCAAQYYYIABBhhggK8E/vPg0Rf7+76KPpZBPu9MgyhlOum8BzEGLhhXfQf0jj4WnDt5q9fs7COMW6leqxgiySgwMtnFAEuhpJbrOFWKbHhwZpbeoXZIdiK1mnGv1VYb3Tx4ZmETNOSE9TxEWPEiyU6BJ4hselIVK1hPBxNA5a0vxk9NUYJ+DNFjZli7NcUL3vliz9Cad146U4sbyBbg0dIWSFLOy3q6UOFe74Yq20CGBc/2QGzDhn2HUane3nb4aD3EftwDGbVFCQYG1VMEayrDdyelH/4X/268A63zr8o0UptUAAAAAElFTkSuQmCC";
// const FRAME_MALVEKE = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAKAAAACQCAMAAAC4a5CyAAAAAXNSR0IB2cksfwAAAAlwSFlzAAALEwAACxMBAJqcGAAAANJQTFRFAAAA////AAAAJycnGBgYEhISGhoaFRUVCAgIMjIyJSUlHBwcMDAwOTk5Dg4OODg4CwsLBQUFICAgHx8fKCgoIiIiISEhAwMDFhYWKSkpBwcHDQ0NGxsbIyMjNjY2LCwsBgYGNDQ0CQkJMzMzMTExHh4eGRkZQEBABAQENTU1CgoKAQEBPDw8Ojo6TU1NFBQUU1NTEBAQNzc3R0dHERERPj4+JCQkQUFBLi4uSkpKAgICOzs7UlJSPz8/KysrRUVFREREFxcXKioqPT09DAwMExMTgz2dlgAAAEZ0Uk5T//8A/////////////////////////////////////////////////////////////////////////////////////////6pvgrQAAALzSURBVHic7dvRdtJAEAbg+Q2laME2KZVCpWIglkK1oggVpdpq3/+VuptkkyUhPd6YDDrTU8LuycV3dnYnIQtEEhUFYF42X6N/AMm7pMUSGJ/GDxifRGYgy48/A6ad5QPjv8STMK1ZF2e2ogzHwISU9qc+VOUrHMG0z6JVlGK7iKAAWOUiSYG0mWO+QGvecSjUBUAy66JqoMTOxrOKQ4ACFKAABShAAQpQgAIUoAAFKEABClCAAvz3gMkzWibA3DNjAQpQgLsOTIIJMDN6YAXMbZyBmAGz26MbG+McgFs36uNtaQ7ADU+uyQNYyOMBRHaD2W4yAWYrDatV/HSwBLJPMftFwr7M2F08gEVERsDCYsMHWBBsgbwWyfYEgxPQeKwRjHq4AI0nN4RcgLkhNCnnAiy8oeECLAyWQFjXFQ5AZK9zAK+bBeSEcS8XoO3JCdkAbaH9zXsOwM1fAuwCkH2ZYQ9k/6FpBwo1rxtW25MAORVq25MCiRmQtgD51EGizJUkPfABkgD/I6AdHIBPhgAFKEABClCAAhSgAAUoQAEKUIACFKAABSjAvwaUkIjCAWq0B9SJ6sA+NZ6HT5QdvKADNBt6r6318hBH+4Dr6dZx+cA2nQCvqOPpt9QKgafoUlc1Q1KviTPgNfV167R84Dm90cBBF3hrgD6GnRECQkuf1MQ7YKCA9ZJ1EfBifKmBE5XcCwOc4mryXiXfAPUAVgNs4wOu8REezfDpMwxQ5bYGzKc6qa4GfnGJFtCztOQAlrjBV8zGagYq2wrwSY8gTuC44RoZ68O3AenD94OygTdo4Qpr1MZ93K7RmK6jFF9iBG/l40dwPe2oKaBW9eoWzcAvG+igucRyqBLZjvbv4xR7+Ak1WmYOzoFZVYukO0RLLWN/hPPDFKhmIO5SoCozTgTslA70Atyr6T/7hbPfEVAT1ZCpMQvr4KKhgAHQC+vgQ8nAGuZ7qN2pMnePnloGbqAVPXKPsfAjYN/vY9ADJg9VACVMPALFgoxq3hHFvgAAAABJRU5ErkJggg==";
const FRAME_SIZE = 3584; // 128 * 112 * 2bpp / 8 bits
const PALETTES = {
    "default": [
        "#ffffff",
        "#aaaaaa",
        "#555555",
        "#000000"
    ],
    "grayscale": [
        "#ffffff",
        "#aaaaaa",
        "#555555",
        "#000000"
    ],
    "dmg": [
        "#9BBC0F",
        "#77A112",
        "#306230",
        "#0F380F"
    ],
    "gameboypocket": [
        "#c4cfa1",
        "#8b956d",
        "#4d533c",
        "#1f1f1f"
    ],
    "gameboycoloreuus": [
        "#ffffff",
        "#7bff30",
        "#0163c6",
        "#000000"
    ],
    "gameboycolorjp": [
        "#ffffff",
        "#ffad63",
        "#833100",
        "#000000"
    ],
    "bgb": [
        "#e0f8d0",
        "#88c070",
        "#346856",
        "#081820"
    ],
    "grafixkidgray": [
        "#e0dbcd",
        "#a89f94",
        "#706b66",
        "#2b2b26"
    ],
    "grafixkidgreen": [
        "#dbf4b4",
        "#abc396",
        "#7b9278",
        "#4c625a"
    ],
    "blackzero": [
        "#7e8416",
        "#577b46",
        "#385d49",
        "#2e463d"
    ]
};

const AsyncButton = defineAsyncComponent(() => import('./components/Button.vue') )
const AsyncSelect = defineAsyncComponent(() => import('./components/Select.vue') )

export default {
  components: {
    AsyncButton,
    AsyncSelect,
  },
  setup() {
    const container = ref(null);
    const canvasRef = ref(null);
    const ctx = ref(null);
    const loading = ref(false);
    const inStreaming = ref(false);
    const source = ref(null);
    const selectedPalette = ref("default");
    const enableDithering = ref(true);
    const withFrame = ref(false);
    const recording = ref(false);
    const rec = ref(null);
    const exposure = ref(1280);
     

    const image = new Image();

    function onChangePalette(evt) {
      selectedPalette.value = evt.target.value;
    }
    function setDimensions() {
      // // let scale = this.$refs.gameboy.offsetWidth * 0.1651376146789;
      // // console.log('scale',scale)

      // const windowWidth = window.innerWidth;
      // const windowHeight = window.innerHeight;
      
      // // Calcula la escala para que el elemento se ajuste al tamaño de la ventana
      // // const scaleX = windowWidth / 350; // 350px es el ancho original
      // const scale = windowHeight / 720; // 570px es el alto original
      // // const scale = Math.min(scaleX, scaleY);
      
      // // Aplica la escala al elemento
      // if(windowHeight < 800)  {
      //   container.value.style.transform = `scale(${scale})`;
      // }
      
    };
    // Convierte los colores de la paleta a valores RGB
    function hexToRgb(hex) {
        let bigint = parseInt(hex.slice(1), 16);
        return [(bigint >> 16) & 255, (bigint >> 8) & 255, bigint & 255];
    }
    function Recorder() {
      let fps = 30;
      let canvas = canvasRef.value;
      // const ctx = canvas.getContext('2d');

      let videoStream = canvas.captureStream(fps);
      let mediaRecorder = new MediaRecorder(videoStream);

      let videoURL;

      let chunks = [];

      function download(dataurl, filename) {
          const link = document.createElement("a");
          link.href = dataurl;
          link.download = filename;
          link.click();
      }

      mediaRecorder.onstop = function(e) {
        let blob = new Blob(chunks, {
          'type': 'video/mp4'
        });
        chunks = [];
        videoURL = URL.createObjectURL(blob);
        let myReader = new FileReader();
        myReader.readAsDataURL(blob);
        myReader.addEventListener("loadend", function(e) {
            download(videoURL, "gb_camera.webm");
        });
      }

      mediaRecorder.ondataavailable = function(e) {
          chunks.push(e.data);
      }

      this.start = () => mediaRecorder.start();
      this.stop = () => mediaRecorder.stop();
      this.pause = () => mediaRecorder.pause();
      this.resume = () => mediaRecorder.resume();
      this.getUrl = () => videoURL;
      this.download = (fileName) => {
          if (videoURL != "")
          download(videoURL, fileName)
      }
    };
    function onOpen(evt) {
      console.log('Events Connected');
    }
    function onError(evt) {
      if (evt.target.readyState != EventSource.OPEN) { 
        console.log('Events Disconnected'); 
        inStreaming.value = false;
        recording.value = false;
      } 
    }
    function onMessage() {}
    function onFrame (message) {
      inStreaming.value = true;
      // Decodificar Base64 a una cadena binaria
      const base64String = message.data;
      const binaryString = atob(base64String);

      // Crear un array de bytes a partir de la cadena binaria
      const byteArray = new Uint8Array(binaryString.length);
      for (let i = 0; i < binaryString.length; i++) {
          byteArray[i] = binaryString.charCodeAt(i);
      }
      // Limpiar el canvas
      ctx.value.clearRect(0, 0, canvasRef.value.width, canvasRef.value.height);
      // El índice de byteArray
      let index = 0;

      ctx.value.drawImage(image, 0, 0);
      // Obtén los datos de la imagen
          let imageData = ctx.value.getImageData(0, 0, canvasRef.value.width, canvasRef.value.height);
          let data = imageData.data;
          
          // //  Invert
          for (var i = 0; i < data.length; i += 4) {
              data[i] = 255 - data[i];         // Componente rojo
              data[i + 1] = 255 - data[i + 1]; // Componente verde
              data[i + 2] = 255 - data[i + 2]; // Componente azul
              // Mantener el componente alfa (transparencia) sin cambios
          }
          const palette = PALETTES[selectedPalette.value];
          let paletteRgb = palette.map(color => hexToRgb(color));
          // Aplica la paleta a cada píxel
          for (let i = 0; i < data.length; i += 4) {
              // Convertir el color original a un índice basado en la intensidad del píxel
              let grayscaleValue = data[i]; // Como es blanco y negro, R=G=B
              let paletteIndex = Math.floor(grayscaleValue / 64); // Ajusta según el número de colores en tu paleta

              // Aplica el color de la paleta
              data[i] = paletteRgb[paletteIndex][0];     // R
              data[i + 1] = paletteRgb[paletteIndex][1]; // G
              data[i + 2] = paletteRgb[paletteIndex][2]; // B
          }

          // Redibuja la imagen con la nueva paleta
          ctx.value.putImageData(imageData, 0, 0);


          // Iterar sobre cada bloque de 8x8 píxeles
          for (let yt = 0; yt < GBCAM_H / 8; yt++) { // Asume 14 bloques de 8 píxeles en alto
              for (let xt = 0; xt < GBCAM_W / 8; xt++) { // Asume 16 bloques de 8 píxeles en ancho
                  // Leer 8 bytes de datos para este bloque
                  for (let y = 0; y < 8; y++) {
                      let data_l = byteArray[index++];
                      let data_h = byteArray[index++];
                      
                      // Dibujar los píxeles del bloque
                      for (let x = 0; x < 8; x++) {
                          // Obtener el color del píxel
                          let pixel = ((data_h & 0x80) >> 7) | ((data_l & 0x80) >> 6);
                          data_l <<= 1;
                          data_h <<= 1;
                          
                          // Establecer el color del píxel
                          const color = PALETTES[selectedPalette.value][pixel];
                          ctx.value.fillStyle = color;
                          
                          // Dibujar el píxel escalado
                          const pixelX = (xt * 8 + x);
                          const pixelY = (yt * 8 + y);
                          
                          const paddingX = withFrame.value ? 16 : 0;
                          const paddingY = withFrame.value ? 16 : 0;

                          // Rellenar el píxel con color
                          ctx.value.fillRect(pixelX + paddingX, pixelY + paddingY, 1, 1);
                          
                          // // Dibujar el borde
                          // ctx.strokeStyle = 'black'; // Color del borde
                          // ctx.lineWidth = borderWidth;
                          // ctx.strokeRect(pixelX, pixelY, 1, 1);
                      }
                  }
              }
          }
    }
    function onChangeExposure(evt) {
      fetch(`/set_exposure?value=${evt.target.value}`)
    }
    function initCanvas() {
      canvasRef.value.width = 128;
      canvasRef.value.height = 112;
      ctx.value = canvasRef.value.getContext('2d');

      window.requestAnimationFrame = (function() {
          return window.requestAnimationFrame ||
          window.webkitRequestAnimationFrame ||
          window.mozRequestAnimationFrame ||
          window.oRequestAnimationFrame ||
          window.msRequestAnimationFrame ||
          function(callback) {
              window.setTimeout(callback, 1000 / 60);
          };
      })();

      image.onload = function() {
        source.value.addEventListener('frame', onFrame, false);
      };
      image.src = FRAME_JP;
    }
    function startStreaming() {
      console.log('startStreaming');
      fetch(`/start_gb_camera`);
    }
    function stopStreaming() {
      console.log('stopStreaming');
      fetch(`/stop_gb_camera`);
      inStreaming.value = false;
      if(rec.value) {
        rec?.stop();
      }
      recording.value = false;
    }
    function onChangeDithering(evt) {
      enableDithering.value = evt.target.checked;
      if(enableDithering.value) {
          fetch(`/enable_dithering`)
      } else {
          fetch(`/disable_dithering`)
      }
    }
    function onChangeFrame(evt) {
      withFrame.value = evt.target.checked ;
        if(withFrame.value) {
            canvasRef.value.width = 160;
            canvasRef.value.height = 144;
        } else {
          canvasRef.value.width = 128;
          canvasRef.value.height = 112;
        }
    }
    function onRecordStream() {
      if(!recording.value) {
          rec.value = new Recorder();
          rec.value?.start();
          console.log("start record");
          recording.value = true;
      } else {
          rec.value?.stop();
          recording.value = false;
      }
    }
    onMounted(() => {
      window.addEventListener("resize", setDimensions);
      if (!!window.EventSource) { 
        source.value = new EventSource('/events'); 
        source.value.addEventListener('open', onOpen, false);
        source.value.addEventListener('error', onError, false);
        source.value.addEventListener('message', onMessage, false);

        initCanvas();
      } else {
        alert("Error:Browser not supported")
      }
      nextTick(() => {
        setDimensions();
      });
    });
    onBeforeUnmount(() => {
      window.removeEventListener("resize", setDimensions);
    });
    return {
      loading,
      selectedPalette,
      canvasRef,
      onChangeExposure,
      onChangePalette,
      container,
      exposure,
      startStreaming,
      stopStreaming,
      inStreaming,
      enableDithering,
      withFrame,
      onChangeDithering,
      onChangeFrame,
      onRecordStream,
      recording
    };
  },
};
</script>

<template>
  <div class="flex flex-col justify-center items-center w-full h-full overflow-hidden pattern">
    
    <div v-if="inStreaming" class="box-width mb-2">
      <AsyncSelect v-model="selectedPalette" @change="onChangePalette">
        <option value="default">Default</option>
        <option value="grayscale">Grayscale</option>
        <option value="dmg">Original Game Boy</option>
        <option value="gameboypocket">Game Boy Pocket</option>
        <option value="gameboycoloreuus">
          Game Boy Color (Game Boy Camera, EU/US)
        </option>
        <option value="gameboycolorjp">
          Game Boy Color (PocketCamera, JP)
        </option>
        <option value="bgb">bgb emulator</option>
        <option value="grafixkidgray">Grafixkid Gray</option>
        <option value="grafixkidgreen">Grafixkid Green</option>
        <option value="blackzero">Game Boy (Black Zero) palette</option>
      </AsyncSelect>
      <div class="flex flex-row gap-2 mt-2 justify-between">
        <div class="flex flex-row gap-2">
          <div class="flex flex-row gap-2 rounded-md bg-gray-200 px-3 py-2 text-[0.8125rem] ">
            <label for="dithering">Dithering</label>
            <input id="dithering" type="checkbox" v-model="enableDithering" @change="onChangeDithering" />
          </div>
          <div class="flex flex-row gap-2 rounded-md bg-gray-200 px-3 py-2 text-[0.8125rem] ">
            <label for="frame">Frame</label>
            <input id="frame" type="checkbox" v-model="withFrame" @change="onChangeFrame" />
          </div>
        </div>
        <div>
          <AsyncButton @click="onRecordStream">{{ recording ? 'STOP RECORD' : 'START RECORD' }}</AsyncButton>
        </div>
      </div>
    </div>
    <div class="box-width box-height lcd flex bg-gray-300">
      <canvas ref="canvasRef" style="width:100%; height:100%;"></canvas>
      <div data-v-7a7a37b1="" style="background-image: url(&quot;/gbcamera-logo.png&quot;);width: 450px;height: 50px;position: absolute;background-size: contain;bottom: -120px;background-repeat: no-repeat;">
      </div>
      <div v-if="inStreaming" class="absolute -right-16">
        <div class="flex flex-col">
          <label class="text-xs" for="slider_exposure">Exposure</label>
          <input type="range" id="slider_exposure" :disabled="!inStreaming" :min="0" :max="43008" :step="128" v-model="exposure" @change="onChangeExposure" />
        </div>
      </div>
    </div>
    <div class="box-width mt-2" style="width: calc(160px * 2.4);">
      <AsyncButton v-if="!inStreaming" @click="startStreaming">START</AsyncButton>
      <AsyncButton v-else @click="stopStreaming">STOP</AsyncButton>
    </div>
    
   
   

   
</div>
</template>

<style lang="scss" scoped>
.transparent-image {
  background-image: linear-gradient(45deg, #b0b0b0 25%, transparent 25%),
    linear-gradient(-45deg, #b0b0b0 25%, transparent 25%),
    linear-gradient(45deg, transparent 75%, #b0b0b0 75%),
    linear-gradient(-45deg, transparent 75%, #b0b0b0 75%);
  background-size: 20px 20px;
  background-position: 0 0, 0 10px, 10px -10px, -10px 0px;
}
canvas {
  // transform: scale(1.64);
  // transform-origin: top left;
  // padding-top: 3px;
}
.pattern::after {
  content: " ";
  background: linear-gradient(90deg, #395a9c 50%, #cfa54a 50%);
  height: 100%;
  width: 100%;
  position: absolute;
  z-index: -1;
}
.box-width {
  width: calc(160px * 2.4);
}
.box-height {
  height: calc(144px * 2.4);
}
.pattern .lcd {
  position: relative;
  width: calc(160px * 2.4);
  height: calc(144px * 2.4);
  display: flex;
  align-items: center;
  justify-content: center;
}
.pattern .lcd::after {
  content: " ";
  position: absolute;
  top: -4px;
  left: -4px;
  width: calc(100% + 8px);
  height: calc(100% + 8px);
  border: 8px solid transparent;
  border-image: linear-gradient(90deg, #5273b5 50%, #e7bd6b 50%);
  border-image-slice: 1;
  z-index: 100;
}
input[type=range] {
    writing-mode: vertical-lr;
    direction: rtl;
    vertical-align: middle;
}
</style>
