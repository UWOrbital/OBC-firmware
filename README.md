# Toast Service
A standardized notification service which responds based on the success or failure of an API call. The service is very flexible and can be called in a variety of situations. A toast will emerge from the top-left, and will last for five seconds. If you hover over it, the toast will not dissapear.

## Usage
#### Import the Service

```TypeScript
import toastService from '../services/toast.service';
```

#### Call the Service

The service should be used within logic that handles asynchronous operations, such as in a try...catch...finally block after an API call.
On Success, call ```toastService.success()``` with a confirmation message.
On Error, call ```toastService.error()``` with an informative error message.

## Testing

Try running the following code added to App.tsx. The dots ```...``` represents the rest of the code.

```TypeScript
...
import toastService from './services/Toast.service'
...

function App() {
 ...
 const showSuccess = () => {
     toastService.success("This is a success message for testing!");
 };
 
 const showError = () => {
     toastService.error("This is an error message for testing.");
 };
 return (
  ...
  {/* --- TEST BUTTONS --- */}
   <div className="w-1/4 flex flex-row mt-[10%] ms-[34.5%]">
       <h3>Toast Test Controls</h3>
       <button onClick={showSuccess} className="bg-lime-400 w-1/2 cursor-pointer">Success</button>
       <button onClick={showError} className="bg-red-400 btn w-1/2 cursor-pointer">Error</button>
   </div>
  {/* --- END --- */}
  ...
 )
}
```

Clicking on the buttons should display the error messages respectively.
