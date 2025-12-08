const {ChartJSNodeCanvas} = require("chartjs-node-canvas");

const w = 900;
const h= 500;

const chart = new ChartJSNodeCanvas({width: w, height: h, backgroundColor: 'white'});
async function generateChart(data) {
    const temps = data.map(d => d.temp);
    const hums = data.map(d => d.humidity);
    const t = data.map((_,i) => i);

    const config = {
        type: "line",
        data: {
            labels: t,
            datasets: [
                {
                    label: "Temperature (°C)",
                    data: temps,
                    borderColor: "red",
                    backgroundColor: "red",
                    tension: 0.3
                },
                {
                    label: "Humidity (%)",
                    data: hums,
                    borderColor: "blue",
                    backgroundColor: "blue",
                    tension: 0.3,
                    yAxisID: "y1"
                }
            ]
        },
        options: {
            responsive: false,
            plugins: {
                title: {
                    display: true,
                    text: "Temperature and Humidity Change Over Time"
                }
            },
            scales: {
                y: {
                    type: "linear",
                    position: "left"
                },
                y1: {
                    type: "linear",
                    position: "right"
                }
            }
        }
    };
    return await chart.renderToBuffer(config);
}
module.exports = generateChart;