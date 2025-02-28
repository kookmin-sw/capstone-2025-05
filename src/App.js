import './App.css';
import { Route, Routes, BrowserRouter } from 'react-router-dom';
import TestPage from './Pages/TestPage/testPage';
import MapleHeader from '../src/Components/MapleHeader';
import MapleFooter from './Components/MapleFooter';

function App() {
  return (
    <div className="App">
      <BrowserRouter>
        <Routes>
          <Route path="/test" element={<TestPage />} />
        </Routes>
      </BrowserRouter>
    </div>
  );
}

export default App;
