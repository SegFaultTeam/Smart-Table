const TelegramBot = require('node-telegram-bot-api'); //including node-library
require("dotenv").config();     
const fs = require("fs");
const dgram = require("dgram");
const PORT = 5000;
const server = dgram.createSocket("udp4");
const token = process.env.BOT_API; //Telegram API
const bot = new TelegramBot(token, {polling: true});

let data = null;
const file = fs.readFileSync("chatId.json").toString();
const chatID = JSON.parse(file).chatId;
server.on('message', (msg) => { //getting data by UDP
    console.log(`${msg}`); //checking if data is received
    data = msg.toString(); //saving data
    if(chatID && data) {
    bot.sendMessage(chatID,data);
}
});

server.bind(PORT); //initting udp connection on port 5000


/*bot.on('message', (msg) => {
    console.log("message is received");
    chatId = msg.chat.id;
    fs.writeFileSync("chatId.json", JSON.stringify({chatId}));
});
*/
