import './App.css';
import { Route, Routes, BrowserRouter } from 'react-router-dom';
import TestPage from './Pages/TestPage/testPage';
import Login from './Pages/Login/login';

function App() {
  return (
    <div className="App">
      <BrowserRouter>
        <Routes>
          <Route path="/test" element={<TestPage />} />
          <Route path="/login" element={<Login />} />
        </Routes>
      </BrowserRouter>
    </div>
  );
}

export default App;
