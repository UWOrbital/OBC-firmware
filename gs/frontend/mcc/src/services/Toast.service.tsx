import { toast, type ToastOptions } from 'react-toastify';

const toastConfig: ToastOptions = {
    position: "top-right",
    autoClose: 5000,
    hideProgressBar: true,
    closeOnClick: true,
    pauseOnHover: true,
    draggable: true,
    progress: undefined,
    theme: 'light',
};

/**
 * Displays a success toast notification.
 * @param message The message to display.
 */
export const success = (message: string): void => {
  toast.success(message, {
    ...toastConfig,
    className: 'toast-success', // Custom class for styling
  });
};

/**
 * Displays an error toast notification.
 * @param message The message to display.
 */
export const error = (message: string): void => {
  toast.error(message, {
    ...toastConfig,
    className: 'toast-error', // Custom class for styling
  });
};

// Export functions individually or as an object
const toastService = {
    success,
    error,
};

export default toastService;