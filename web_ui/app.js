/* ========== ESP32-C6 AI Web UI Controller ========== */

(function () {
    'use strict';

    const translations = {
        zh: {
            'nav.chat': '聊天', 'nav.logs': '日志', 'nav.settings': '设置', 'nav.about': '关于',
            'header.online': '已联网', 'header.ap_mode': 'AP 模式',
            'chat.sidebar_title': '会话', 'chat.input_placeholder': '输入消息...',
            'chat.new_title': '聊天', 'chat.welcome': '你好，我是 ESP32-C6 AI 助手。',
            'logs.boot': '系统启动完成', 'logs.ui_loaded': 'Web UI 加载完成',
            'logs.send_msg': '发送消息', 'logs.new_chat': '新建聊天', 'logs.chat_deleted': '聊天已删除',
            'logs.delete_confirm': '确定删除这个聊天吗？',
            'logs.wifi_saved': '已保存 WiFi', 'logs.wifi_deleted': '已删除 WiFi',
            'logs.connecting_wifi': '正在连接 WiFi', 'logs.connect_started': '已发起连接', 'logs.connect_failed': '连接失败',
            'logs.ap_saved': 'AP 设置已保存', 'logs.ap_save_failed': 'AP 设置保存失败',
            'logs.lang_switched': '语言已切换', 'logs.ai_saved': 'AI 配置已保存', 'logs.ai_save_failed': 'AI 配置保存失败',
            'logs.log_level_saved': '日志等级已切换', 'logs.log_level_failed': '日志等级切换失败',
            'logs.rgb_saved': 'RGB 设置已保存', 'logs.rgb_save_failed': 'RGB 设置保存失败',
            'logs.load_chat_list_failed': '加载聊天列表失败', 'logs.load_chat_failed': '加载聊天记录失败',
            'logs.load_wifi_failed': '加载 WiFi 列表失败', 'logs.save_wifi_failed': '保存 WiFi 失败',
            'logs.status_failed': '获取状态失败', 'logs.system_status_failed': '获取系统状态失败',
            'logs.load_ai_failed': '加载 AI 配置失败', 'logs.load_log_level_failed': '加载日志等级失败', 'logs.load_rgb_failed': '加载 RGB 设置失败',
            'settings.language.title': '语言 / Language',
            'settings.status.title': '系统状态', 'settings.status.cpu': 'CPU 占用', 'settings.status.mem': '内存 占用', 'settings.status.flash': 'Flash 占用',
            'settings.rgb.title': 'RGB 灯', 'settings.rgb.red': '红', 'settings.rgb.green': '绿', 'settings.rgb.blue': '蓝', 'settings.rgb.gradient': '渐变', 'settings.rgb.off': '关闭', 'settings.rgb.pin': '数据引脚', 'settings.rgb.save_btn': '保存 RGB 设置',
            'settings.network.title': '网络状态', 'settings.network.sta_status': 'STA 状态', 'settings.network.sta_ip': 'STA IP',
            'settings.network.connected': '已连接', 'settings.network.disconnected': '未连接',
            'settings.ai.title': 'AI 配置', 'settings.ai.api_url': 'API 地址', 'settings.ai.api_url_placeholder': 'https://api.example.com/v1/chat/completions',
            'settings.ai.api_key': 'API Key', 'settings.ai.api_key_placeholder': '输入 API Key',
            'settings.ai.personality': '人格设置', 'settings.ai.personality_placeholder': '输入系统提示词 / 人格设定', 'settings.ai.save_btn': '保存 AI 配置',
            'settings.log.title': '日志等级', 'settings.log.off': '关闭', 'settings.log.error': 'Error', 'settings.log.info': 'Info', 'settings.log.debug': 'Debug',
            'settings.wifi.title': 'WiFi 连接', 'settings.wifi.ssid': 'WiFi 名称', 'settings.wifi.ssid_placeholder': '输入 WiFi SSID',
            'settings.wifi.password': '密码', 'settings.wifi.password_placeholder': '输入密码',
            'settings.wifi.add_btn': '添加并保存', 'settings.wifi.saved_title': '已保存的网络', 'settings.wifi.empty_ssid': '请输入 WiFi 名称',
            'settings.wifi.scan_title': '附近网络', 'settings.wifi.scan_btn': '扫描', 'settings.wifi.scan_empty': '未发现网络', 'settings.wifi.scan_failed': '扫描失败',
            'settings.ap.title': 'AP 热点', 'settings.ap.ssid': '热点名称', 'settings.ap.password': '热点密码', 'settings.ap.save_btn': '保存并重启 AP',
            'settings.ap.empty_ssid': '请输入热点名称', 'settings.ap.short_password': '密码至少需要 8 位',
            'about.version': '固件版本 v0.1.0',
            'about.desc': '基于 ESP32-C6 的轻量级 AI API 调用固件。通过 AP 热点提供 Web 交互界面，支持多 WiFi 自动连接与网络共享。',
            'about.chip': '芯片', 'about.ui': 'UI 风格', 'about.flash': 'Flash',
            'wifi.connect': '连接', 'wifi.delete': '删除', 'wifi.delete_confirm': '确定删除 WiFi "{0}" 吗？',
            'ai.reply': '消息已收到，AI 回复功能待接入后端。'
        },
        en: {
            'nav.chat': 'Chat', 'nav.logs': 'Logs', 'nav.settings': 'Settings', 'nav.about': 'About',
            'header.online': 'Online', 'header.ap_mode': 'AP Mode',
            'chat.sidebar_title': 'Chats', 'chat.input_placeholder': 'Type a message...',
            'chat.new_title': 'Chat', 'chat.welcome': 'Hello, I am the ESP32-C6 AI assistant.',
            'logs.boot': 'System boot complete', 'logs.ui_loaded': 'Web UI loaded',
            'logs.send_msg': 'Message sent', 'logs.new_chat': 'New chat created', 'logs.chat_deleted': 'Chat deleted',
            'logs.delete_confirm': 'Delete this chat?',
            'logs.wifi_saved': 'WiFi saved', 'logs.wifi_deleted': 'WiFi deleted',
            'logs.connecting_wifi': 'Connecting to WiFi', 'logs.connect_started': 'Connection started', 'logs.connect_failed': 'Connection failed',
            'logs.ap_saved': 'AP settings saved', 'logs.ap_save_failed': 'Failed to save AP settings',
            'logs.lang_switched': 'Language switched', 'logs.ai_saved': 'AI config saved', 'logs.ai_save_failed': 'Failed to save AI config',
            'logs.log_level_saved': 'Log level switched', 'logs.log_level_failed': 'Failed to switch log level',
            'logs.rgb_saved': 'RGB settings saved', 'logs.rgb_save_failed': 'Failed to save RGB settings',
            'logs.load_chat_list_failed': 'Failed to load chat list', 'logs.load_chat_failed': 'Failed to load chat history',
            'logs.load_wifi_failed': 'Failed to load WiFi list', 'logs.save_wifi_failed': 'Failed to save WiFi',
            'logs.status_failed': 'Failed to get status', 'logs.system_status_failed': 'Failed to get system status',
            'logs.load_ai_failed': 'Failed to load AI config', 'logs.load_log_level_failed': 'Failed to load log level', 'logs.load_rgb_failed': 'Failed to load RGB settings',
            'settings.language.title': 'Language / 语言',
            'settings.status.title': 'System Status', 'settings.status.cpu': 'CPU Usage', 'settings.status.mem': 'Memory Usage', 'settings.status.flash': 'Flash Usage',
            'settings.rgb.title': 'RGB LED', 'settings.rgb.red': 'Red', 'settings.rgb.green': 'Green', 'settings.rgb.blue': 'Blue', 'settings.rgb.gradient': 'Gradient', 'settings.rgb.off': 'Off', 'settings.rgb.pin': 'Data Pin', 'settings.rgb.save_btn': 'Save RGB Settings',
            'settings.network.title': 'Network Status', 'settings.network.sta_status': 'STA Status', 'settings.network.sta_ip': 'STA IP',
            'settings.network.connected': 'Connected', 'settings.network.disconnected': 'Disconnected',
            'settings.ai.title': 'AI Config', 'settings.ai.api_url': 'API URL', 'settings.ai.api_url_placeholder': 'https://api.example.com/v1/chat/completions',
            'settings.ai.api_key': 'API Key', 'settings.ai.api_key_placeholder': 'Enter API Key',
            'settings.ai.personality': 'Personality', 'settings.ai.personality_placeholder': 'Enter system prompt / personality', 'settings.ai.save_btn': 'Save AI Config',
            'settings.log.title': 'Log Level', 'settings.log.off': 'Off', 'settings.log.error': 'Error', 'settings.log.info': 'Info', 'settings.log.debug': 'Debug',
            'settings.wifi.title': 'WiFi Connection', 'settings.wifi.ssid': 'WiFi Name', 'settings.wifi.ssid_placeholder': 'Enter WiFi SSID',
            'settings.wifi.password': 'Password', 'settings.wifi.password_placeholder': 'Enter password',
            'settings.wifi.add_btn': 'Add & Save', 'settings.wifi.saved_title': 'Saved Networks', 'settings.wifi.empty_ssid': 'Please enter WiFi name',
            'settings.wifi.scan_title': 'Nearby Networks', 'settings.wifi.scan_btn': 'Scan', 'settings.wifi.scan_empty': 'No networks found', 'settings.wifi.scan_failed': 'Scan failed',
            'settings.ap.title': 'AP Hotspot', 'settings.ap.ssid': 'Hotspot Name', 'settings.ap.password': 'Hotspot Password', 'settings.ap.save_btn': 'Save & Restart AP',
            'settings.ap.empty_ssid': 'Please enter hotspot name', 'settings.ap.short_password': 'Password must be at least 8 characters',
            'about.version': 'Firmware v0.1.0',
            'about.desc': 'A lightweight AI API calling firmware based on ESP32-C6. Provides a Web UI via AP hotspot, supporting multi-WiFi auto-connection and network sharing.',
            'about.chip': 'Chip', 'about.ui': 'UI Style', 'about.flash': 'Flash',
            'wifi.connect': 'Connect', 'wifi.delete': 'Delete', 'wifi.delete_confirm': 'Delete WiFi "{0}"?',
            'ai.reply': 'Message received, AI reply feature will connect to backend later.'
        }
    };

    let currentLang = 'zh';

    function t(key, ...args) {
        let text = translations[currentLang]?.[key] || translations.zh[key] || key;
        args.forEach((arg, i) => { text = text.replace(`{${i}}`, arg); });
        return text;
    }

    function applyLanguage() {
        document.documentElement.lang = currentLang === 'zh' ? 'zh-CN' : 'en';
        document.querySelectorAll('[data-i18n]').forEach(el => el.textContent = t(el.getAttribute('data-i18n')));
        document.querySelectorAll('[data-i18n-placeholder]').forEach(el => el.placeholder = t(el.getAttribute('data-i18n-placeholder')));
        document.querySelectorAll('.lang-btn').forEach(btn => btn.classList.toggle('active', btn.dataset.lang === currentLang));
        renderChatList();
        loadWifiList();
        updateNetworkStatus();
    }

    async function setLanguage(lang) {
        if (lang !== 'zh' && lang !== 'en') return;
        currentLang = lang;
        applyLanguage();
        try {
            await apiPost('/api/lang/set', { language: lang });
            addLog(t('logs.lang_switched') + ': ' + lang);
        } catch (e) {
            addLog('Failed to save language', 'error');
        }
    }

    async function apiGet(path) {
        const res = await fetch(path);
        return res.json();
    }

    async function apiPost(path, body) {
        const res = await fetch(path, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(body)
        });
        return res.json();
    }

    async function saveConfig(path, body, successKey, errorKey, type = 'success') {
        try {
            const res = await apiPost(path, body);
            addLog(t(res.success ? successKey : errorKey), res.success ? type : 'error');
            return res.success;
        } catch (e) {
            addLog(t(errorKey), 'error');
            return false;
        }
    }

    async function loadConfig(path, setter, errorKey) {
        try {
            setter(await apiGet(path));
        } catch (e) {
            addLog(t(errorKey), 'error');
        }
    }

    const navItems = document.querySelectorAll('.nav-item');
    const pages = document.querySelectorAll('.page');

    function switchPage(pageId) {
        pages.forEach(page => page.classList.toggle('active', page.id === pageId));
        navItems.forEach(item => item.classList.toggle('active', item.dataset.page === pageId));
    }

    navItems.forEach(item => item.addEventListener('click', () => switchPage(item.dataset.page)));

    function getTimeString() {
        return new Date().toLocaleTimeString(currentLang === 'zh' ? 'zh-CN' : 'en-US', { hour12: false, hour: '2-digit', minute: '2-digit', second: '2-digit' });
    }

    function escapeHtml(text) {
        const div = document.createElement('div');
        div.textContent = text;
        return div.innerHTML;
    }

    function generateId() {
        return Date.now().toString(36) + Math.random().toString(36).substr(2, 5);
    }

    function formatBytes(bytes) {
        if (bytes < 1024) return bytes + ' B';
        if (bytes < 1024 * 1024) return (bytes / 1024).toFixed(1) + ' KB';
        return (bytes / (1024 * 1024)).toFixed(2) + ' MB';
    }

    const logList = document.getElementById('log-list');

    function addLog(text, type = 'info') {
        const item = document.createElement('div');
        item.className = `log-item ${type}`;
        item.innerHTML = `<span class="log-time">${getTimeString()}</span><span class="log-text">${escapeHtml(text)}</span>`;
        logList.appendChild(item);
        logList.scrollTop = logList.scrollHeight;
    }

    const chatInput = document.getElementById('chat-input');
    const chatSend = document.getElementById('chat-send');
    const chatMessages = document.getElementById('chat-messages');
    const chatListEl = document.getElementById('chat-list');
    const btnNewChat = document.getElementById('btn-new-chat');

    let chats = {};
    let currentChatId = null;

    function renderChatList() {
        chatListEl.innerHTML = '';
        Object.keys(chats).forEach(id => {
            const item = document.createElement('div');
            item.className = `chat-item ${id === currentChatId ? 'active' : ''}`;
            item.innerHTML = `<span>${escapeHtml(chats[id].title)}</span><button class="del-chat" data-id="${id}">×</button>`;
            item.querySelector('span').addEventListener('click', () => loadChat(id));
            item.querySelector('.del-chat').addEventListener('click', (e) => { e.stopPropagation(); deleteChat(id); });
            chatListEl.appendChild(item);
        });
    }

    function renderMessages() {
        chatMessages.innerHTML = '';
        if (!currentChatId || !chats[currentChatId]) return;
        chats[currentChatId].messages.forEach(msg => addMessageToDOM(msg.text, msg.sender, msg.time, false));
    }

    function addMessageToDOM(text, sender, time, save = true) {
        const msg = document.createElement('div');
        msg.className = `message ${sender}`;
        msg.innerHTML = `<div class="message-bubble">${escapeHtml(text)}</div><div class="message-time">${time || getTimeString()}</div>`;
        chatMessages.appendChild(msg);
        chatMessages.scrollTop = chatMessages.scrollHeight;

        if (save && currentChatId) {
            chats[currentChatId].messages.push({ text, sender, time: time || getTimeString() });
            saveCurrentChat();
        }
    }

    async function saveCurrentChat() {
        if (!currentChatId) return;
        await apiPost('/api/chat/save', { id: currentChatId, messages: chats[currentChatId].messages });
    }

    function createChat() {
        const id = generateId();
        const title = `${t('chat.new_title')} ${Object.keys(chats).length + 1}`;
        chats[id] = { title, messages: [{ text: t('chat.welcome'), sender: 'bot', time: getTimeString() }] };
        loadChat(id);
        renderChatList();
        saveCurrentChat();
        addLog(t('logs.new_chat') + ': ' + title);
    }

    async function loadChat(id) {
        currentChatId = id;
        try {
            const messages = await apiGet(`/api/chat/get?id=${id}`);
            chats[id].messages = Array.isArray(messages) && messages.length > 0 ? messages : [{ text: t('chat.welcome'), sender: 'bot', time: getTimeString() }];
        } catch (e) {
            addLog(t('logs.load_chat_failed'), 'error');
        }
        renderChatList();
        renderMessages();
    }

    async function deleteChat(id) {
        if (!confirm(t('logs.delete_confirm'))) return;
        if ((await apiPost('/api/chat/delete', { id })).success) {
            delete chats[id];
            if (currentChatId === id) {
                const ids = Object.keys(chats);
                currentChatId = ids.length > 0 ? ids[0] : null;
            }
            renderChatList();
            renderMessages();
            addLog(t('logs.chat_deleted'));
        }
    }

    async function loadChatList() {
        try {
            const list = await apiGet('/api/chat/list');
            chats = {};
            list.forEach(c => { chats[c.id] = { title: c.title || `${t('chat.new_title')} ${c.id.substr(0, 4)}`, messages: [] }; });
            if (!currentChatId || !chats[currentChatId]) {
                const ids = Object.keys(chats);
                currentChatId = ids.length > 0 ? ids[0] : null;
            }
            renderChatList();
            if (currentChatId) await loadChat(currentChatId);
        } catch (e) {
            addLog(t('logs.load_chat_list_failed'), 'error');
        }
    }

    function sendChat() {
        const text = chatInput.value.trim();
        if (!text || !currentChatId) return;
        addMessageToDOM(text, 'user');
        chatInput.value = '';
        addLog(t('logs.send_msg'));
        setTimeout(() => addMessageToDOM(t('ai.reply'), 'bot'), 600);
    }

    chatSend.addEventListener('click', sendChat);
    chatInput.addEventListener('keypress', (e) => { if (e.key === 'Enter') sendChat(); });
    btnNewChat.addEventListener('click', createChat);

    const wifiSsid = document.getElementById('wifi-ssid');
    const wifiPassword = document.getElementById('wifi-password');
    const btnAddWifi = document.getElementById('btn-add-wifi');
    const btnScanWifi = document.getElementById('btn-scan-wifi');
    const wifiListEl = document.getElementById('wifi-list');
    const wifiScanListEl = document.getElementById('wifi-scan-list');

    async function loadWifiList() {
        try {
            const list = await apiGet('/api/wifi/list');
            wifiListEl.innerHTML = '';
            list.forEach(w => {
                const item = document.createElement('div');
                item.className = 'wifi-item';
                item.innerHTML = `<span class="wifi-name" title="${t('wifi.connect')}">${escapeHtml(w.ssid)}</span><div class="wifi-actions"><button class="btn-connect" title="${t('wifi.connect')}">▶</button><button class="btn-delete" title="${t('wifi.delete')}">✕</button></div>`;
                item.querySelector('.wifi-name').addEventListener('click', () => connectWifi(w.ssid, w.password));
                item.querySelector('.btn-connect').addEventListener('click', () => connectWifi(w.ssid, w.password));
                item.querySelector('.btn-delete').addEventListener('click', () => removeWifi(w.ssid));
                wifiListEl.appendChild(item);
            });
        } catch (e) {
            addLog(t('logs.load_wifi_failed'), 'error');
        }
    }

    async function addWifi() {
        const ssid = wifiSsid.value.trim();
        if (!ssid) return alert(t('settings.wifi.empty_ssid'));
        if (await saveConfig('/api/wifi/add', { ssid, password: wifiPassword.value }, 'logs.wifi_saved', 'logs.save_wifi_failed')) {
            wifiSsid.value = '';
            wifiPassword.value = '';
            loadWifiList();
        }
    }

    async function removeWifi(ssid) {
        if (!confirm(t('wifi.delete_confirm', ssid))) return;
        if (await saveConfig('/api/wifi/delete', { ssid }, 'logs.wifi_deleted', 'logs.save_wifi_failed')) loadWifiList();
    }

    async function connectWifi(ssid, password) {
        addLog(`${t('logs.connecting_wifi')}: ${ssid}`);
        await saveConfig('/api/wifi/connect', { ssid, password }, 'logs.connect_started', 'logs.connect_failed');
    }

    async function scanWifi() {
        btnScanWifi.disabled = true;
        btnScanWifi.textContent = '...';
        try {
            const list = await apiGet('/api/wifi/scan');
            wifiScanListEl.innerHTML = '';
            if (!list.length) {
                wifiScanListEl.innerHTML = `<div class="wifi-empty">${t('settings.wifi.scan_empty')}</div>`;
                return;
            }
            list.forEach(ap => {
                const item = document.createElement('div');
                item.className = 'wifi-item';
                const rssiClass = ap.rssi >= -60 ? 'good' : (ap.rssi >= -75 ? 'ok' : 'weak');
                item.innerHTML = `<span class="wifi-name" title="${t('wifi.connect')}">${escapeHtml(ap.ssid)} <small>(${ap.auth})</small></span><div class="wifi-actions"><span class="wifi-rssi ${rssiClass}">${ap.rssi}dBm</span><button class="btn-connect" title="${t('wifi.connect')}">▶</button></div>`;
                item.querySelector('.wifi-name').addEventListener('click', () => promptConnectScanned(ap.ssid, ap.auth));
                item.querySelector('.btn-connect').addEventListener('click', () => promptConnectScanned(ap.ssid, ap.auth));
                wifiScanListEl.appendChild(item);
            });
        } catch (e) {
            addLog(t('settings.wifi.scan_failed'), 'error');
        } finally {
            btnScanWifi.disabled = false;
            btnScanWifi.textContent = t('settings.wifi.scan_btn');
        }
    }

    function promptConnectScanned(ssid, auth) {
        if (auth === 'OPEN') {
            connectWifi(ssid, '');
        } else {
            const password = prompt(`${t('settings.wifi.password')} (${ssid})`, '');
            if (password === null) return;
            connectWifi(ssid, password);
        }
    }

    btnAddWifi.addEventListener('click', addWifi);
    btnScanWifi.addEventListener('click', scanWifi);

    const aiApiUrl = document.getElementById('ai-api-url');
    const aiApiKey = document.getElementById('ai-api-key');
    const aiPersonality = document.getElementById('ai-personality');
    const btnSaveAi = document.getElementById('btn-save-ai');

    async function loadAiConfig() {
        await loadConfig('/api/ai', cfg => {
            aiApiUrl.value = cfg.api_url || '';
            aiApiKey.value = cfg.api_key || '';
            aiPersonality.value = cfg.personality || '';
        }, 'logs.load_ai_failed');
    }

    async function saveAiConfig() {
        await saveConfig('/api/ai', {
            api_url: aiApiUrl.value.trim(),
            api_key: aiApiKey.value,
            personality: aiPersonality.value.trim()
        }, 'logs.ai_saved', 'logs.ai_save_failed');
    }

    btnSaveAi.addEventListener('click', saveAiConfig);

    const rgbModeButtons = document.querySelectorAll('.rgb-btn');
    const rgbPinInput = document.getElementById('rgb-pin');
    const btnSaveRgb = document.getElementById('btn-save-rgb');
    let currentRgbMode = 0;

    function updateRgbButtons(mode) {
        currentRgbMode = mode;
        rgbModeButtons.forEach(btn => btn.classList.toggle('active', parseInt(btn.dataset.mode) === mode));
    }

    async function loadRgbConfig() {
        await loadConfig('/api/rgb', cfg => {
            updateRgbButtons(cfg.mode ?? 0);
            rgbPinInput.value = cfg.pin ?? 8;
        }, 'logs.load_rgb_failed');
    }

    async function saveRgbConfig() {
        const pin = parseInt(rgbPinInput.value);
        if (isNaN(pin) || pin < 0 || pin > 48) return alert('Pin must be 0-48');
        await saveConfig('/api/rgb', { mode: currentRgbMode, pin }, 'logs.rgb_saved', 'logs.rgb_save_failed');
    }

    rgbModeButtons.forEach(btn => btn.addEventListener('click', () => updateRgbButtons(parseInt(btn.dataset.mode))));
    btnSaveRgb.addEventListener('click', saveRgbConfig);

    const logLevelRadios = document.querySelectorAll('input[name="log_level"]');

    async function loadLogLevel() {
        await loadConfig('/api/log_level', data => {
            const level = data.level ?? 2;
            logLevelRadios.forEach(r => r.checked = parseInt(r.value) === level);
        }, 'logs.load_log_level_failed');
    }

    logLevelRadios.forEach(r => r.addEventListener('change', () => {
        if (r.checked) saveConfig('/api/log_level', { level: parseInt(r.value) }, 'logs.log_level_saved', 'logs.log_level_failed');
    }));

    const metricCpuValue = document.getElementById('metric-cpu-value');
    const metricCpuBar = document.getElementById('metric-cpu-bar');
    const metricMemValue = document.getElementById('metric-mem-value');
    const metricMemBar = document.getElementById('metric-mem-bar');
    const metricFlashValue = document.getElementById('metric-flash-value');
    const metricFlashBar = document.getElementById('metric-flash-bar');

    let lastFlashUpdate = 0;

    function updateProgressBar(bar, percent) {
        bar.style.width = percent + '%';
        bar.classList.remove('warning', 'danger');
        if (percent >= 90) bar.classList.add('danger');
        else if (percent >= 70) bar.classList.add('warning');
    }

    async function loadSystemStatus() {
        try {
            const status = await apiGet('/api/system_status');
            const now = Date.now();

            metricCpuValue.textContent = status.cpu.percent + '%';
            updateProgressBar(metricCpuBar, status.cpu.percent);

            metricMemValue.textContent = status.mem.percent + '% (' + formatBytes(status.mem.free) + ' / ' + formatBytes(status.mem.total) + ')';
            updateProgressBar(metricMemBar, status.mem.percent);

            if (now - lastFlashUpdate >= 300000) {
                metricFlashValue.textContent = status.flash.percent + '% (' + formatBytes(status.flash.used) + ' / ' + formatBytes(status.flash.total) + ')';
                updateProgressBar(metricFlashBar, status.flash.percent);
                lastFlashUpdate = now;
            }
        } catch (e) {
            addLog(t('logs.system_status_failed'), 'error');
        }
    }

    const apSsid = document.getElementById('ap-ssid');
    const apPassword = document.getElementById('ap-password');
    const btnSaveAp = document.getElementById('btn-save-ap');
    const headerStatus = document.getElementById('header-status');

    let lastStatus = {};

    function updateNetworkStatus() {
        headerStatus.textContent = lastStatus.sta_connected ? t('header.online') : t('header.ap_mode');
        document.querySelector('.status-dot').classList.toggle('offline', !lastStatus.sta_connected);
    }

    async function loadStatus() {
        try {
            lastStatus = await apiGet('/api/status');
            if (lastStatus.language && (lastStatus.language === 'zh' || lastStatus.language === 'en')) currentLang = lastStatus.language;
            apSsid.value = lastStatus.ap?.ssid || '';
            apPassword.value = lastStatus.ap?.password || '';
            applyLanguage();
        } catch (e) {
            addLog(t('logs.status_failed'), 'error');
        }
    }

    async function saveAp() {
        const ssid = apSsid.value.trim();
        const password = apPassword.value;
        if (!ssid) return alert(t('settings.ap.empty_ssid'));
        if (password && password.length < 8) return alert(t('settings.ap.short_password'));
        await saveConfig('/api/ap/set', { ssid, password }, 'logs.ap_saved', 'logs.ap_save_failed');
    }

    btnSaveAp.addEventListener('click', saveAp);

    document.querySelectorAll('.lang-btn').forEach(btn => btn.addEventListener('click', () => setLanguage(btn.dataset.lang)));

    async function init() {
        addLog(t('logs.ui_loaded'));
        await loadStatus();
        await loadSystemStatus();
        await loadAiConfig();
        await loadLogLevel();
        await loadRgbConfig();
        await loadWifiList();
        await loadChatList();
        setInterval(loadStatus, 5000);
        setInterval(loadSystemStatus, 3000);
    }

    init();

})();
