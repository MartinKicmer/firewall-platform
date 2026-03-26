export type TrafficLayer = 'L2' | 'L3' | 'L4Simple' | 'L4TCP'

export type TrafficAction = 0 | 1

export type TrafficControlSubmit = {
  action: TrafficAction
  layer: TrafficLayer
  count: number | 'all'
}

export type TrafficRow = {
  timestamp?: string
  layer?: string
  action?: 'ALLOW' | 'DENY'
  source?: string
  destination?: string
  sourcePort?: number | string
  destinationPort?: number | string
  protocol?: string
  info?: string
  raw?: Record<string, unknown>
}