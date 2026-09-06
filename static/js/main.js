// Function to fetch live Arduino data from the Flask backend
function fetchLiveData() {
    // Flask handles the route seamlessly on the same domain
    fetch('/api/dynamic.json') 
        .then(response => {
            if (!response.ok) {
                throw new Error('Network response was not ok');
            }
            return response.json();
        })
        .then(data => {
            // IMPORTANT: Code with this format and put in all additional code here.
            // Example: If your JSON looks like { "temperature": 24 }
            // document.getElementById('temp-display').innerText = data.temperature + "°C";
            // console.log("Updated data:", data);
        })
        .catch(error => console.error('Error fetching live data:', error));
}

// Fetch data immediately when the page loads
document.addEventListener('DOMContentLoaded', () => {
    fetchLiveData();
    setInterval(fetchLiveData, 1000); 
});
