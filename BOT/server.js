const TelegramBot = require('node-telegram-bot-api'); //including node-library
require("dotenv").config();     
const fs = require("fs");
const {SerialPort} = require("serialport");
const {ReadlineParser} = require("@serialport/parser-readline");
const generateChart = require("./chart");
const token = process.env.BOT_API; //Telegram API
const bot = new TelegramBot(token, {polling: true});
const file = fs.readFileSync("chatId.json").toString();
let chatID = JSON.parse(file).chatID; //getting chatId
let time = 0; //time
const statistic = [];
const pattern = /^\s*\d+(\.\d+)?\s*,\s*\d+(\.\d+)?\s*$/; //pattern for "a, b" numbers type
const patternForNumber = /^\s*\d+(\.\d+)?\s*$/; //pattern for signle number
let started = false;
class Weather {
    constructor(temp, humidity) {
        this.temp = temp;
        this.humidity = humidity;
    }
}
const port = new SerialPort({
    path:"/dev/ttyACM0",
    baudRate: 115200,
});

const parser = port.pipe(new ReadlineParser({delimiter: "\n"}));
parser.on("data", (msg) => {
    if(!msg.trim()) return;
    console.log(`${msg}`); //checking if data is received
    const data = msg.toString(); //saving data
    if(patternForNumber.test(data)) {
        time = Number(data) * 1000;
        started = true;
        return;
    }
   else if(pattern.test(data)) {
        const [a,b] = data.split(",").map(num => Number(num.trim()));
        statistic.push(new Weather(a ,b));
    }
    if(chatID && data) {
    bot.sendMessage(chatID,data);
    }
});


//drawing statistic
async function getStatistic() {
    const imgBuffer = await generateChart(statistic);
    bot.sendPhoto(chatID, imgBuffer);
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
