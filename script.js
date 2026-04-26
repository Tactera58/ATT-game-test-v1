// Define time zones with their cities
const timeZones = [
    { city: 'New York', timezone: 'America/New_York' },
    { city: 'Los Angeles', timezone: 'America/Los_Angeles' },
    { city: 'London', timezone: 'Europe/London' },
    { city: 'Paris', timezone: 'Europe/Paris' },
    { city: 'Tokyo', timezone: 'Asia/Tokyo' },
    { city: 'Sydney', timezone: 'Australia/Sydney' },
    { city: 'Dubai', timezone: 'Asia/Dubai' },
    { city: 'Singapore', timezone: 'Asia/Singapore' },
    { city: 'Hong Kong', timezone: 'Asia/Hong_Kong' },
    { city: 'Mumbai', timezone: 'Asia/Kolkata' },
    { city: 'Toronto', timezone: 'America/Toronto' },
    { city: 'Mexico City', timezone: 'America/Mexico_City' }
];

// Format time with leading zeros
function formatTime(hours, minutes, seconds) {
    return `${String(hours).padStart(2, '0')}:${String(minutes).padStart(2, '0')}:${String(seconds).padStart(2, '0')}`;
}

// Format date
function formatDate(date) {
    const options = { weekday: 'short', year: 'numeric', month: 'short', day: 'numeric' };
    return date.toLocaleDateString('en-US', options);
}

// Create clock card HTML
function createClockCard(city, timezone) {
    return `
        <div class="clock-card">
            <div class="timezone">${timezone}</div>
            <div class="city">${city}</div>
            <div class="time" id="time-${city}">00:00:00</div>
            <div class="date" id="date-${city}">Loading...</div>
        </div>
    `;
}

// Initialize clocks
function initializeClocks() {
    const grid = document.querySelector('.clocks-grid');
    
    timeZones.forEach(({ city, timezone }) => {
        grid.innerHTML += createClockCard(city, timezone);
    });
    
    updateAllClocks();
}

// Update all clocks
function updateAllClocks() {
    timeZones.forEach(({ city, timezone }) => {
        const now = new Date();
        
        // Get time in specific timezone
        const timeInZone = new Date(now.toLocaleString('en-US', { timeZone: timezone }));
        
        const hours = timeInZone.getHours();
        const minutes = timeInZone.getMinutes();
        const seconds = timeInZone.getSeconds();
        
        // Update time element
        const timeElement = document.getElementById(`time-${city}`);
        if (timeElement) {
            timeElement.textContent = formatTime(hours, minutes, seconds);
        }
        
        // Update date element
        const dateElement = document.getElementById(`date-${city}`);
        if (dateElement) {
            dateElement.textContent = formatDate(timeInZone);
        }
    });
}

// Update clocks every second
setInterval(updateAllClocks, 1000);

// Initialize when DOM is ready
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', initializeClocks);
} else {
    initializeClocks();
}