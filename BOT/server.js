const TelegramBot = require('node-telegram-bot-api'); //including node-library
require("dotenv").config();     
const fs = require("fs");
const dgram = require("dgram");
const {SerialPort} = require("serialport");
const {ReadlineParser} = require("@serialport/parser-readline");

const token = process.env.BOT_API; //Telegram API
const bot = new TelegramBot(token, {polling: true});
const file = fs.readFileSync("chatId.json").toString();
let chatID = JSON.parse(file).chatId;

// PLAN A\
/*
const PORT = 5000;
const server = dgram.createSocket("udp4");
server.on('message', (msg) => { //getting data by UDP
    console.log(`${msg}`); //checking if data is received
    const data = msg.toString(); //saving data
    if(chatID && data) {
    bot.sendMessage(chatID,data);
}
});

server.bind(PORT); 
*///initting udp connection on port 5000
// Plan B
const port = new SerialPort({
    path:"/dev/ttyACM0",
    baudRate: 115200,
});

const parser = port.pipe(new ReadlineParser({delimiter: "\n"}));

parser.on("data", (msg) => {
    if(!msg.trim()) return;
    console.log(`${msg}`); //checking if data is received
    const data = msg.toString(); //saving data
    if(chatID && data) {
    bot.sendMessage(chatID,data);
}
})

bot.on('message', (msg) => {
    console.log("Message is received");
    chatID = msg.chat.id;
    fs.writeFileSync("chatId.json", JSON.stringify({chatID}));
});

