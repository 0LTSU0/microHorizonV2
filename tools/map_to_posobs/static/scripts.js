var map = L.map('map_container').setView([65.011366, 25.469219], 13);
L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
    maxZoom: 19,
    attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
}).addTo(map);

var posObsAngle = 0;
const cursor = document.createElement('img');
cursor.src = 'static/arrow-up2-svgrepo-com.svg';
cursor.id = 'posobs_cursor';
cursor.style.display = 'none';
document.body.appendChild(cursor);
const cursorSize = 32; //needs to correspond to syle.css so that the cursor is aligned with our real cursor properly

document.addEventListener('mousemove', (e) => {
    // Only update if cursor is visible
    if (cursor.style.display === 'block') {
        cursor.style.left = `${e.clientX - cursorSize / 2}px`;
        cursor.style.top = `${e.clientY - cursorSize / 2}px`;
    }
});

const mapContainer = document.getElementById('map_container');

mapContainer.addEventListener('mouseenter', () => {
    cursor.style.display = 'block';
});

mapContainer.addEventListener('mouseleave', () => {
    cursor.style.display = 'none';
});

document.addEventListener('keydown', (e) => {
    if (e.key === 'a' || e.key === 'ArrowLeft') {
        posObsAngle -= 10;
        console.debug("Current cursor rotation:", posObsAngle)
    } else if (e.key === 'd' || e.key === 'ArrowRight') {
        posObsAngle += 10;
        console.debug("Current cursor rotation:", posObsAngle)
    }
    cursor.style.transform = `rotate(${posObsAngle}deg)`;
});

var posObsArray = [];
var arrowIcon = L.icon({
    iconUrl: 'static/arrow-up2-svgrepo-com.svg',
    iconSize: [32, 32],
    iconAnchor: [16, 16]
})
map.on('click', function(e) {
    console.log("Clicked map @", e.latlng);
    let posObsMarker = L.marker([e.latlng.lat, e.latlng.lng], {
        rotationAngle: posObsAngle,
        icon: arrowIcon
    }).addTo(map);
    posObsArray.push(posObsMarker)
    sendPosObs(e.latlng, posObsAngle)
});

function clearMap() {
    for (const marker of posObsArray) {
        map.removeLayer(marker);
    }
    posObsArray = [];
}

function sendPosObs(coord, heading_raw) {
    let heading = 0
    if (heading_raw < 0) {
        heading = heading_raw % -360
    } else {
        heading = heading_raw % 360
    }
    if (heading < 0) {
        heading = 360 + heading;
    }
    let data = {
        lat: coord.lat,
        lon: coord.lng,
        heading: heading
    }
    console.log("Sending pos obs:", data)
    fetch("/postPosObs", {
        method: "POST",
        headers: {'Content-Type': 'application/json'}, 
        body: JSON.stringify(data)
    }).then(res => {
        console.log("Response from sending posobs:", res);
    });
}
