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

class Weather {
    constructor(temp, humidity) {
        this.temp = temp;
        this.humidity = humidity;
    }
}

const statistic = [];
statistic.push(new Weather(20, 50));
statistic.push(new Weather(30, 40));
statistic.push(new Weather(35, 60));
statistic.push(new Weather(33, 20));
statistic.push(new Weather(32, 70));
statistic.push(new Weather(18, 80));
statistic.push(new Weather(10, 10));
statistic.push(new Weather(15, 5));
const pattern = /^\s*\d+(\.\d+)?\s*,\s*\d+(\.\d+)?\s*$/; //pattern for "a, b" numbers type
const patternForNumber = /^\s*\d+(\.\d+)?\s*$/; //pattern for signle number
let time = 10000; //time

//drawing statistic
async function getStatistic() {
    const imgBuffer = await generateChart(statistic);
    bot.sendPhoto(chatID, imgBuffer);
}

// sending statistic every time interval
setInterval(getStatistic, time);

//getting user's chatID, in case it is empty.
bot.on('message', (msg) => {
    console.log("Message is received");
    chatID = msg.chat.id;
    fs.writeFileSync("chatId.json", JSON.stringify({chatID}));
});
