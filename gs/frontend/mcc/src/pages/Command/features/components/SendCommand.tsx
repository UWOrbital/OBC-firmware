import React, { useState, useEffect } from 'react';
import { useAppSelector } from '../../../../store/hooks';
import { selectCommand } from '../selectCommandSlice';
import { mockCommandsList, type ExtendedCommand, type CommandParameter } from '../../../../utils/mock-data';
import { Button } from "@/components/ui/button"
import {
  Field,
  FieldDescription,
  FieldGroup,
  FieldLabel,
  FieldLegend,
  // FieldSeparator,
  FieldSet,
} from "@/components/ui/field"
import { Input } from "@/components/ui/input"
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select"

interface ParameterValues {
  [key: string]: string;
}

/**
 * @brief SendCommand component for displaying and submitting command parameters
 * @return tsx element of SendCommand component
 */
function SendCommand() {
  const selectedCommandName = useAppSelector(selectCommand);
  const [matchedCommand, setMatchedCommand] = useState<ExtendedCommand | null>(null);
  const [parameterValues, setParameterValues] = useState<ParameterValues>({});
  const [isSubmitting, setIsSubmitting] = useState(false);
  // const [submitMessage, setSubmitMessage] = useState<string>('');

  // Effect to detect selectedCommand changes and match to mock data
  useEffect(() => {
    if (selectedCommandName != "") {
      const command = mockCommandsList.find(cmd => cmd.name === selectedCommandName);
      if (command) {
        setMatchedCommand(command);
        // Initialize parameter values
        const initialValues: ParameterValues = {};
        command.parameters.forEach(param => {
          initialValues[param.name] = '';
        });
        setParameterValues(initialValues);
      } else {
        setMatchedCommand(null);
        setParameterValues({});
      }
    } else {
      setMatchedCommand(null);
      setParameterValues({});
    }
  }, [selectedCommandName]);

  // Handle input changes
  const handleParameterChange = (paramName: string, value: string) => {
    setParameterValues(prev => ({
      ...prev,
      [paramName]: value
    }));
  };

  // Validate input based on parameter type
  const validateParameter = (param: CommandParameter, value: string): boolean => {
    if (!value.trim()) {
      return false;
    }

    if (!value.trim()) return true; // Optional empty fields are valid

    switch (param.type) {
      case 'int': {
        const intValue = parseInt(value);
        if (isNaN(intValue)) return false;
        // Check byte size constraints if specified
        if (param.size) {
          const maxValue = Math.pow(2, param.size * 8) - 1;
          if (intValue < 0 || intValue > maxValue) return false;
        }
        return true;
      }
      case 'float': {
        return !isNaN(parseFloat(value));
      }
      case 'boolean': {
        return value.toLowerCase() === 'true' || value.toLowerCase() === 'false';
      }
      case 'string': {
        return true;
      }
      default: {
        return true;
      }
    }
  };

  // Check if all required parameters are valid
  const isFormValid = (): boolean => {
    if (!matchedCommand) return false;

    return matchedCommand.parameters.every(param => {
      const value = parameterValues[param.name] || '';
      return validateParameter(param, value);
    });
  };

  // Handle form submission
  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();

    if (!matchedCommand || !isFormValid()) {
      // setSubmitMessage('Please fill in all required fields with valid values.');
      return;
    }

    setIsSubmitting(true);
    // setSubmitMessage('');

    try {
      // Prepare submission data
      const submissionData = {
        commandId: matchedCommand.id,
        commandName: matchedCommand.name,
        parameters: parameterValues
      };

      // Simulate API call (replace with actual API call)
      await new Promise(resolve => setTimeout(resolve, 1000));

      console.log('Submitting command:', submissionData);

      // TODO: Replace with actual API call
      // const response = await fetch('/api/commands', {
      //   method: 'POST',
      //   headers: { 'Content-Type': 'application/json' },
      //   body: JSON.stringify(submissionData)
      // });

      // setSubmitMessage(`Command "${matchedCommand.name}" submitted successfully!`);

      // Clear form after successful submission
      const initialValues: ParameterValues = {};
      matchedCommand.parameters.forEach(param => {
        initialValues[param.name] = '';
      });
      setParameterValues(initialValues);

    } catch (error) {
      console.error('Error submitting command:', error);
      // setSubmitMessage('Error submitting command. Please try again.');
    } finally {
      setIsSubmitting(false);
    }
  };

  // Render parameter input based on type
  const renderParameterInput = (param: CommandParameter) => {
    const value = parameterValues[param.name] || '';
    const isValid = validateParameter(param, value);
    const inputId = `param-${param.name}`;

    const baseInputClasses = `
      w-full px-3 py-2 border rounded-md
      focus:outline-none focus:ring-2 focus:ring-blue-500
      ${!isValid && value ? 'border-red-500 bg-red-50' : 'border-gray-300'}
    `;

    switch (param.type) {
      case 'boolean':
        return (
          <Select
            value={value}
            onValueChange={(val) => handleParameterChange(param.name, val)}
          >
            <SelectTrigger className={`w-[180px] ${!isValid && value ? 'border-red-500 bg-red-50' : 'border-gray-300'}`}>
              <SelectValue placeholder="Select..." />
            </SelectTrigger>
            <SelectContent>
              <SelectItem value="true">True</SelectItem>
              <SelectItem value="false">False</SelectItem>
            </SelectContent>
          </Select>
        );

      case 'int':
      case 'float':
        return (
          <Input
            id={inputId}
            type="number"
            value={value}
            onChange={(e) => handleParameterChange(param.name, e.target.value)}
            className={baseInputClasses}
            placeholder={param.type === 'int' ? 'Enter integer' : 'Enter decimal number'}
            step={param.type === 'float' ? 'any' : '1'}
          />
        );

      default:
        return (
          <Input
            id={inputId}
            type="text"
            value={value}
            onChange={(e) => handleParameterChange(param.name, e.target.value)}
            className={baseInputClasses}
            placeholder="Enter text"
          />
        );
    }
  };

  if (!matchedCommand) {
    return
  }

  return (
    // <div className="rounded-lg shadow-md p-6 w-2xl">
    //   <h2 className="text-2xl font-bold mb-4 text-gray-800">
    //     Configure Command: {matchedCommand.name}
    //   </h2>

    //   <div className="mb-4 p-4 bg-gray-50 rounded-lg">
    //     <h3 className="font-semibold text-gray-700 mb-2">Command Details:</h3>
    //     <p><span className="font-medium">ID:</span> {matchedCommand.id}</p>
    //     <p><span className="font-medium">Format:</span> {matchedCommand.format}</p>
    //     <p><span className="font-medium">Data Size:</span> {matchedCommand.data_size} bytes</p>
    //     <p><span className="font-medium">Total Size:</span> {matchedCommand.total_size} bytes</p>
    //   </div>

    //   <form onSubmit={handleSubmit} className="space-y-4">
    //     <h3 className="text-lg font-semibold text-gray-700 mb-3">Parameters:</h3>

    //     {matchedCommand.parameters.map((param, index) => {
    //       const value = parameterValues[param.name] || '';
    //       const isValid = validateParameter(param, value);

    //       return (
    //         <div key={index} className="space-y-2">
    //           <label
    //             htmlFor={`param-${param.name}`}
    //             className="block text-sm font-medium text-gray-700"
    //           >
    //             {param.name}
    //             {<span className="text-red-500 ml-1">*</span>}
    //             <span className="text-gray-500 ml-2">({param.type})</span>
    //             {param.size && (
    //               <span className="text-gray-500 ml-1">
    //                 - {param.size} byte{param.size !== 1 ? 's' : ''}
    //               </span>
    //             )}
    //           </label>

    //           {renderParameterInput(param)}

    //           {!isValid && value && (
    //             <p className="text-sm text-red-600">
    //               Invalid {param.type} value
    //               {param.size && param.type === 'int' && (
    //                 ` (must be 0-${Math.pow(2, param.size * 8) - 1})`
    //               )}
    //             </p>
    //           )}
    //         </div>
    //       );
    //     })}

    //     <div className="flex items-center justify-between pt-4">
    //       <button
    //         type="submit"
    //         disabled={!isFormValid() || isSubmitting}
    //         className={`
    //           px-6 py-2 rounded-md font-medium transition-colors
    //           ${isFormValid() && !isSubmitting
    //             ? 'bg-blue-600 hover:bg-blue-700 text-white'
    //             : 'bg-gray-300 text-gray-500 cursor-not-allowed'
    //           }
    //         `}
    //       >
    //         {isSubmitting ? 'Sending...' : 'Send Command'}
    //       </button>

    //       {submitMessage && (
    //         <div className={`text-sm ${
    //           submitMessage.includes('Error') || submitMessage.includes('Please fill')
    //             ? 'text-red-600'
    //             : 'text-green-600'
    //         }`}>
    //           {submitMessage}
    //         </div>
    //       )}
    //     </div>
    //   </form>
    // </div>
    <div>
      <form onSubmit={handleSubmit}>
        <FieldGroup>
          <FieldSet>
            <FieldLegend>{ matchedCommand.name }</FieldLegend>
            <FieldDescription>
              Command ID: { matchedCommand.id } | Format: { matchedCommand.format } | Data Size: { matchedCommand.data_size } bytes | Total Size: { matchedCommand.total_size } bytes
            </FieldDescription>
            <FieldGroup>

              {matchedCommand.parameters.map((param, index) => {
                const value = parameterValues[param.name] || '';
                const isValid = validateParameter(param, value);
                return (
                <Field key={index}>
                  <FieldLabel htmlFor={`param-${param.name}`}>
                    {param.name}
                  </FieldLabel>
                  {renderParameterInput(param)}
                  {!isValid && value && (
                    <p className="text-sm text-red-600">
                      Invalid {param.type} value
                      {param.size && param.type === 'int' && (
                        ` (must be 0-${Math.pow(2, param.size * 8) - 1})`
                      )}
                    </p>
                  )}
                </Field>
                )
              })}
            </FieldGroup>
          </FieldSet>
          <Field orientation="horizontal">
            <Button type="submit" disabled={!isFormValid() || isSubmitting}>Submit</Button>
            <Button variant="outline" type="button">
              Cancel
            </Button>
          </Field>
        </FieldGroup>
      </form>
    </div>
  );
}

export default SendCommand;
