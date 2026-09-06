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
