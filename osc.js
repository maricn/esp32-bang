const OSC = require("osc-js");

const osc = new OSC({
  plugin: new OSC.DatagramPlugin({
    send: { host: "192.168.178.41", port: 3333 },
  }),
});

async function sequence() {
  const bang = new OSC.Message("/bang");
  osc.send(bang);
  await sleep(100);

  let i = 0;

  osc.send(new OSC.Message("/decay", 0.9999992));
  while (true) {
    osc.send(new OSC.Message("/line"));
    // osc.send(new OSC.Message("/color", "red", 0x1f));
    await sleep(16);
    // osc.send(new OSC.Message("/color", "green", 0x1f));
    // await sleep(280);
    // osc.send(new OSC.Message("/color", "blue", 0x1f));
    // await sleep(280);
    if (i++ % 128 == 0) {
      osc.send(new OSC.Message("/bang"));
    }
  }

  while (true) {
    for (let c of ["red", "green", "blue"]) {
      for (let i = 32; i >= 0; i--) {
        const color = new OSC.Message("/color", c, i);
        osc.send(color);
        await sleep(100);
      }
    }
  }

  osc.send(bang);
}

function sleep(ms) {
  return new Promise((resolve) => {
    setTimeout(resolve, ms);
  });
}

osc.send(new OSC.Message("/decay", 0.4999999));
sequence();
