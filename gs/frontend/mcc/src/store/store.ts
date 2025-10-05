import { configureStore } from '@reduxjs/toolkit'
import commandReducer from '../pages/Command/features/selectCommandSlice.ts'

const store = configureStore({
  reducer: {
    command: commandReducer,
  },
})

export type RootState = ReturnType<typeof store.getState>
export type AppDispatch = typeof store.dispatch

export default store
