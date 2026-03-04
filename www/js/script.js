document.addEventListener('DOMContentLoaded', () => {
    const container = document.querySelector('.container');
    const footer = document.querySelector('.footer');
    
    // Create a button to test interactivity
    const button = document.createElement('button');
    button.textContent = 'Toggle Theme';
    button.style.marginTop = '1.5rem';
    button.style.padding = '0.6rem 1.2rem';
    button.style.backgroundColor = '#007bff';
    button.style.color = '#fff';
    button.style.border = 'none';
    button.style.borderRadius = '6px';
    button.style.cursor = 'pointer';
    button.style.fontSize = '1rem';
    button.style.transition = 'background-color 0.2s';

    button.onmouseover = () => button.style.backgroundColor = '#0056b3';
    button.onmouseout = () => button.style.backgroundColor = '#007bff';

    button.addEventListener('click', () => {
        const isDark = document.body.style.backgroundColor === 'rgb(33, 37, 41)';
        if (!isDark) {
            document.body.style.backgroundColor = '#212529';
            document.body.style.color = '#f8f9fa';
            container.style.backgroundColor = '#343a40';
            container.style.color = '#f8f9fa';
            footer.style.color = '#adb5bd';
        } else {
            document.body.style.backgroundColor = '#f0f2f5';
            document.body.style.color = '#333';
            container.style.backgroundColor = '#fff';
            container.style.color = '#333';
            footer.style.color = '#777';
        }
    });

    container.appendChild(button);

    console.log('WebServ Test Script Loaded Successfully');
});
