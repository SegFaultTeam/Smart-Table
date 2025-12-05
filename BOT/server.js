const TelegramBot = require('node-telegram-bot-api');
require("dotenv").config();
const token = process.env.BOT_API;

const bot = new TelegramBot(token, {polling: true});

bot.on('message', (msg) => {
    bot.sendMessage(msg.chat.id, "TEST");
});

