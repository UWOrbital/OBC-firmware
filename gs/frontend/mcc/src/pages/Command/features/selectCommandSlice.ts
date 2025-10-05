import { createSlice } from '@reduxjs/toolkit'
import type { PayloadAction } from '@reduxjs/toolkit'
import type { RootState } from '../../../store/store'

// Define a type for the command state
interface CommandState {
  value: string
}

// Define the initial state using that type
const initialState: CommandState = {
  value: "",
}

export const commandSlice = createSlice({
  name: 'command',
  // `createSlice` will infer the state type from the `initialState` argument
  initialState,
  reducers: {
    setCommand: (state, action: PayloadAction<string>) => {
      state.value = action.payload
    },
  },
})

export const { setCommand } = commandSlice.actions

// Other code such as selectors can use the imported `RootState` type
export const selectCommand = (state: RootState) => state.command.value

export default commandSlice.reducer
