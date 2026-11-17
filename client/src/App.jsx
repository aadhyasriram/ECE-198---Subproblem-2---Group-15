import { useEffect, useState } from "react";
import "./App.css";

const sampleResidents = [
  {
    id: "resident-a",
    name: "Resident A",
    exercises: [
      {
        name: "Bicep curls",
        sets: 2,
        reps: 20,
        heartRate: 104,
        status: "Completed",
      },
      {
        name: "Shoulder press",
        sets: 1,
        reps: 10,
        heartRate: 112,
        status: "In progress",
      },
      {
        name: "Cardio",
        sets: 1,
        reps: 0,
        heartRate: 126,
        status: "Completed",
      },
    ],
  },
  {
    id: "resident-b",
    name: "Resident B",
    exercises: [
      {
        name: "Leg raises",
        sets: 3,
        reps: 30,
        heartRate: 98,
        status: "Completed",
      },
    ],
  },
];

function App() {
  const [residents, setResidents] = useState(sampleResidents);
  const [selectedId, setSelectedId] = useState(sampleResidents[0].id);
  const [connection, setConnection] = useState("Sample data");

  useEffect(() => {
    fetch("http://localhost:3001/api/workouts")
      .then((response) => {
        if (!response.ok) throw new Error("API unavailable");
        return response.json();
      })
      .then((data) => {
        if (data.residents?.length) {
          setResidents(data.residents);
          setSelectedId(data.residents[0].id);
          setConnection(data.connected ? "Arduino connected" : "API connected");
        }
      })
      .catch(() => {
        // Keep sample data visible when the backend isn't running.
        setConnection("Sample data");
      });
  }, []);

  const resident =
    residents.find((item) => item.id === selectedId) ?? residents[0];
  const exercises = resident?.exercises ?? [];

  const completedCount = exercises.filter(
    (exercise) => exercise.status === "Completed",
  ).length;
  const totalSets = exercises.reduce((sum, exercise) => sum + exercise.sets, 0);
  const totalReps = exercises.reduce((sum, exercise) => sum + exercise.reps, 0);
  const peakHeartRate = Math.max(
    0,
    ...exercises.map((exercise) => exercise.heartRate ?? 0),
  );

  return (
    <main className="dashboard">
      <header className="header">
        <div>
          <p className="eyebrow">CCCare</p>
          <h1>Exercise dashboard</h1>
        </div>
        <span className="connection">{connection}</span>
      </header>

      <section className="filters">
        <label htmlFor="resident">Resident</label>
        <select
          id="resident"
          value={selectedId}
          onChange={(event) => setSelectedId(event.target.value)}
        >
          {residents.map((item) => (
            <option key={item.id} value={item.id}>
              {item.name}
            </option>
          ))}
        </select>
        <span>{new Date().toLocaleDateString()}</span>
      </section>

      <section className="summary">
        <SummaryCard
          label="Exercises completed"
          value={`${completedCount} / ${exercises.length}`}
        />
        <SummaryCard label="Sets completed" value={totalSets} />
        <SummaryCard label="Reps completed" value={totalReps} />
        <SummaryCard
          label="Peak heart rate"
          value={peakHeartRate ? `${peakHeartRate} bpm` : "—"}
        />
      </section>

      <section className="panel">
        <p className="eyebrow">Session details</p>
        <h2>{resident?.name ?? "No resident selected"}</h2>

        {exercises.length === 0 ? (
          <p>No exercise records yet.</p>
        ) : (
          <div className="exercise-list">
            {exercises.map((exercise) => (
              <article className="exercise" key={exercise.name}>
                <strong>{exercise.name}</strong>
                <span>{exercise.sets} sets</span>
                <span>
                  {exercise.reps ? `${exercise.reps} reps` : "Timed activity"}
                </span>
                <span>
                  {exercise.heartRate == null
                    ? "—"
                    : `${exercise.heartRate} bpm`}
                </span>
                <span
                  className={
                    exercise.status === "Completed"
                      ? "status"
                      : "status in-progress"
                  }
                >
                  {exercise.status}
                </span>
              </article>
            ))}
          </div>
        )}
      </section>

      <p className="note">
        Sample project data. Heart-rate values are displayed for exercise
        tracking and are not medical advice.
      </p>
    </main>
  );
}

function SummaryCard({ label, value }) {
  return (
    <article className="summary-card">
      <p>{label}</p>
      <strong>{value}</strong>
    </article>
  );
}

export default App;