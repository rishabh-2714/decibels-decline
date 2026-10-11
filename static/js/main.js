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
            update('6a'); update('6b'); update('6A'); update('6B'); update('6C');
            update('7A'); update('7B'); update('7y'); update('7r');
            update('8A'); update('8B'); update('8s'); update('8g'); update('8h');
            update('9'); update('X'); update('11'); update('12');
            
            // IMPORTANT: Code with this format and put in all additional code here.
            // Example: If your JSON looks like `{ "temperature": 24 }`,
            // document.getElementById('temp-display').innerText = data.temperature + "°C";
            // console.log("Updated data:", data);
        })
        .catch(error => console.error('Error fetching live data:', error));
}
function update(key) {
    document.getElementById(key + 'v').text = data[key];
}

// Fetch data immediately when the page loads
document.addEventListener('DOMContentLoaded', () => {
    fetchLiveData();
    setInterval(fetchLiveData, 1000); 
});
//==========================================
//WEB COMPONENTS
//==========================================
class VolumeC extends HTMLElement {
    constructor() {
        super();
        this.attachShadow({ mode: 'open' });
    }

    static get observedAttributes() {
        return ['text', 'color'];
    }

    connectedCallback() {
        this.render();
    }

    attributeChangedCallback() {
        this.render();
    }

    render() {
        const text = this.getAttribute('text') || '';
        const color = this.getAttribute('color') || '#e2e8f0';

        this.shadowRoot.innerHTML = `
            <style>
                :host {
                    display: inline-block;
                    margin: 5px;
                }
                .circle {
                    display: flex;
                    align-items: center;
                    justify-content: center;
                    height: 50px;
                    width: 50px;
                    border: 1px solid black;
                    border-radius: 50%;
                    font-family: sans-serif;
                    font-size: 12px;
                    font-weight: bold;
                    color: white;
                    text-align: center;
                    transition: background-color 0.3s ease;
                }
            </style>
            <div class="circle" style="background-color: ${color};">
                ${text}
            </div>
        `;
    }
}

customElements.define('volume-c', VolumeC);
