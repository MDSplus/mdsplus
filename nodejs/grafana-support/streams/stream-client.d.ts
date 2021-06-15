export function connectToStreams(port: numeric): Promise<string>;
export function registerStream(signal: string, from: numeric, to: numeric, update: function): numeric;
export function unregisterStream(id: numeric);
