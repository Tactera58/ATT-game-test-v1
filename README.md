# Digital Clock - Multiple Time Zones

A beautiful, responsive web application that displays the current time across 12 major cities in different time zones.

## Features

- ⏰ **Real-time Clock Updates**: Updates every second
- 🌍 **12 Major Cities**: Shows time zones from around the world
- 🎨 **Modern Design**: Beautiful gradient background with glassmorphism cards
- 📱 **Responsive Layout**: Works on desktop, tablet, and mobile devices
- ✨ **Smooth Animations**: Hover effects and smooth transitions
- 📅 **Date Display**: Shows current date in each timezone

## Supported Time Zones

1. New York (America/New_York)
2. Los Angeles (America/Los_Angeles)
3. London (Europe/London)
4. Paris (Europe/Paris)
5. Tokyo (Asia/Tokyo)
6. Sydney (Australia/Sydney)
7. Dubai (Asia/Dubai)
8. Singapore (Asia/Singapore)
9. Hong Kong (Asia/Hong_Kong)
10. Mumbai (Asia/Kolkata)
11. Toronto (America/Toronto)
12. Mexico City (America/Mexico_City)

## How to Use

1. Open `index.html` in your web browser
2. The clocks will automatically update every second
3. Hover over cards to see the hover effect

## Files

- `index.html` - Main HTML structure
- `styles.css` - Styling with glassmorphism design
- `script.js` - JavaScript logic for time updates
- `README.md` - This file

## Technologies Used

- HTML5
- CSS3 (Grid, Flexbox, Gradients, Backdrop Filter)
- JavaScript (Intl API for timezone support)

## Customization

To add more time zones:
1. Edit the `timeZones` array in `script.js`
2. Add a new object with `city` and `timezone` properties
3. Use valid IANA timezone identifiers (e.g., 'Asia/Bangkok')

## Browser Support

Works in all modern browsers that support:
- CSS Grid
- CSS Backdrop Filter
- JavaScript Intl API

## Future Enhancements

- [ ] Add ability to customize time zones
- [ ] Add analog clock display option
- [ ] Add 12/24 hour format toggle
- [ ] Add timezone search functionality
- [ ] Add sound notifications
