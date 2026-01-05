const TelegramBot = require('node-telegram-bot-api'); //including node-library
require("dotenv").config();     //including .env module
const fs = require("fs"); //including file system
const {SerialPort} = require("serialport"); // including serial port reading
const {ReadlineParser} = require("@serialport/parser-readline"); //including parser 
const generateChart = require("./chart"); //adding chart generation
const token = process.env.BOT_API; //Telegram API
const bot = new TelegramBot(token, {polling: true}); //creating new bot
const file = fs.readFileSync("chatId.json").toString(); //reading chat id from json
let chatID = JSON.parse(file).chatID; //getting chatId
let time = 0; //time
const statistic = [];
const pattern = /^\s*\d+(\.\d+)?\s+\d+(\.\d+)?\s*$/; //pattern for "a, b" numbers type
const patternForNumber = /^\s*\d+(\.\d+)?\s*$/; //pattern for signle number
let started = false;
let error = false;

class Weather {
    constructor(temp, humidity) {
        this.temp = temp;
        this.humidity = humidity;
    }
}

const port = new SerialPort({ //creting new port
    path:"/dev/ttyACM0",
    baudRate: 115200,
});

const parser = port.pipe(new ReadlineParser({delimiter: "\n"})); //read messages that splitted by \n
parser.on("data", (msg) => {
    if(!msg.trim()) return;
    console.log(`${msg}`); //checking if data is received
    const data = msg.toString().trim(); //saving data
    if(patternForNumber.test(data)) { //if data is single number (in this case in could be only time)
        time = Number(data) * 1000 * 60;
        console.log("TIME IS ", time);
        if(!started) {
        started = true;
        setInterval(getStatistic, time);
        }
        return;
    }
   else if(pattern.test(data)) { //if data is temp and humidity
        const [a,b] = data.trim().split(/\s+/).map(num => Number(num.trim()));
        console.log(b, a);
        statistic.push(new Weather(a ,b)); //adding data into array of structures
        return;
    }
    if(chatID && data) {
        if(data === "D" || data === "B") error = true; //if data is error code
        else error = false;
        const hasFloat = /-?\d+\.\d+/.test(data);
        if(hasFloat) return;
        const match = data.match(/-?\b\d+\b/);
    bot.sendMessage(chatID,data === "D" ? "DHT sensor is not responding" : data === "B" ? "DHT_RESULT_BAD_CHECKSUM" : data);
    }
});


//drawing statistic
async function getStatistic() {
    if(statistic.length === 0 || error) return;
    console.log(error);
    console.log("Drawing ...", statistic);
    const imgBuffer = await generateChart(statistic); //asynchronous function of generating chart 
    bot.sendPhoto(chatID, imgBuffer); //sending chart
    statistic.splice(0, statistic.length); //resetting statistic array
}

// sending statistic every time interval
if(started) {
setInterval(getStatistic, time);
}

//getting user's chatID, in case it is empty.
bot.on('message', (msg) => {
    console.log("Message is received");
    chatID = msg.chat.id;
    fs.writeFileSync("chatId.json", JSON.stringify({chatID}));
});
